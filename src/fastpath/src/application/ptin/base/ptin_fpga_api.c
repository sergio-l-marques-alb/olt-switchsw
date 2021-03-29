/*********************************************************************
*
* (C) Copyright PT Inova��o S.A. 2014-2014
*
**********************************************************************
*
* @filename ptin_fpga_api.c
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
#include "ptin_fpga_api.h"
#include "commdefs.h"
#include "addrmap.h"
#include "logger.h"

/* FPGA map */
#ifdef MAP_FPGA
volatile st_fpga_map_t *fpga_map = MAP_FAILED;
#endif

#ifdef MAP_CPLD
volatile st_cpld_map_t *cpld_map = MAP_FAILED;
#endif

/**
 * Maps FPGA and PLD registers map
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_fpga_init(void)
{
#ifdef MAP_FPGA
  TAddrMap fpga_AddrMap;

  PT_LOG_TRACE(LOG_CTX_STARTUP, "Going to map FPGA...");

  // Load FPGA
  #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  fpga_map = (volatile st_fpga_map_t *) AddrAlloc64((void *) &fpga_AddrMap, (long long) FPGA_BASE_ADDR, sizeof(st_fpga_map_t));
  #else
  fpga_map = (volatile st_fpga_map_t *) AddrAlloc((void *) &fpga_AddrMap, (int) FPGA_BASE_ADDR, sizeof(st_fpga_map_t));
  #endif

  if (fpga_map != MAP_FAILED)
  {
    /* If FPGA id is not valid, free FPGA map */
    if ((fpga_map->map[FPGA_ID1_REG] != FPGA_ID1_VAL) ||
        (fpga_map->map[FPGA_ID0_REG] != FPGA_ID0_VAL)) {

      PT_LOG_ERR(LOG_CTX_STARTUP, "Invalid FPGA ID: 0x%02X%02X (expecting 0x%02X%02X)",
              fpga_map->map[FPGA_ID0_REG], fpga_map->map[FPGA_ID1_REG],
              FPGA_ID0_VAL, FPGA_ID1_VAL);

      AddrFree(&fpga_AddrMap);
      fpga_map = MAP_FAILED;
    }
    /* Otherwise, make some initializations */
    else
    {
#if (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)
      /* Release External PHYs RESET */
      fpga_map->map[FPGA_EXTPHY_RESET] = 0xFF;
      /* Disable all TX */
      fpga_map->map[FPGA_TXDISABLE_REG] = 0x00;
#endif

      PT_LOG_TRACE(LOG_CTX_STARTUP, "FPGA mapping ok");
      PT_LOG_TRACE(LOG_CTX_STARTUP, "  FPGA Id:      0x%02X%02X", fpga_map->map[FPGA_ID0_REG], fpga_map->map[FPGA_ID1_REG]);
      PT_LOG_TRACE(LOG_CTX_STARTUP, "  FPGA Version: %d", fpga_map->map[FPGA_VER_REG]);
    }
  }

  if ( fpga_map == MAP_FAILED )
    return L7_FAILURE;
#endif

#ifdef MAP_CPLD
  TAddrMap cpld_AddrMap;

  PT_LOG_TRACE(LOG_CTX_STARTUP, "Going to map PLD...");

  // Load CPLD
  #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  PT_LOG_TRACE(LOG_CTX_STARTUP, "64 bit platform");
  cpld_map = (volatile st_cpld_map_t *) AddrAlloc64((void *) &cpld_AddrMap, (long long) CPLD_BASE_ADDR, sizeof(st_cpld_map_t));
  #else
  PT_LOG_TRACE(LOG_CTX_STARTUP, "32 bit platform");
  cpld_map = (volatile st_cpld_map_t *) AddrAlloc((void *) &cpld_AddrMap, (int) CPLD_BASE_ADDR, sizeof(st_cpld_map_t));
  #endif

  if (cpld_map != MAP_FAILED)
  {
    /* If CPLD id is not valid, free CPLD map */
    PT_LOG_WARN(LOG_CTX_STARTUP, "CPLD ID is not being validated");

    PT_LOG_TRACE(LOG_CTX_STARTUP, "CPLD mapping ok");
    PT_LOG_TRACE(LOG_CTX_STARTUP, "  CPLD Id:      0x%02X%02X", cpld_map->map[CPLD_ID0_REG], cpld_map->map[CPLD_ID1_REG]);
    PT_LOG_TRACE(LOG_CTX_STARTUP, "  CPLD Version: %d", cpld_map->map[CPLD_VER_REG]);
    PT_LOG_TRACE(LOG_CTX_STARTUP, "  Hw Id:        %d", cpld_map->map[CPLD_HW_ID_REG]);
    PT_LOG_TRACE(LOG_CTX_STARTUP, "  Chassis Id:   %d", cpld_map->map[CPLD_CHASSIS_ID_REG]);
    PT_LOG_TRACE(LOG_CTX_STARTUP, "  Slot Id:      %d", cpld_map->map[CPLD_SLOT_ID_REG]);
    /* No initializations to be done */
  }

  if ( cpld_map == MAP_FAILED )
    return L7_FAILURE;
#endif

  CPLD_INIT();
  FPGA_INIT();

  return L7_SUCCESS;
}


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
* @comments 
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fpga_mx_is_matrixactive(void)
{
  return (CPLD_SLOT_MATRIX_GET() != 0);
}

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
L7_uint8 ptin_fpga_mx_is_matrixactive_rt(void)
{
  if (ptin_fpga_mx_get_matrixactive()==PTIN_SLOT_WORK && !ptin_fpga_mx_is_matrix_in_workingslot())
  {
    /* Running on Stanby matrix */
    return 0;
  }
  if (ptin_fpga_mx_get_matrixactive()==PTIN_SLOT_PROT && ptin_fpga_mx_is_matrix_in_workingslot())
  {
    /* Running on Stanby matrix */
    return 0;
  }

  /* Running on Active matrix */
  return 1;
}

/**
 * Check if current Matrix is the Working one (slot 1) 
 * (Only for Matrix board)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_mx_is_matrix_in_workingslot(void)
{
  return (CPLD_SLOT_ID_GET() == 0);
}
#endif // (PTIN_BOARD_IS_MATRIX)

#if (PTIN_BOARD_IS_LINECARD)
/**
 * Check if Active Matrix is in Working slot 
 * (Only for linecards)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_lc_is_matrixactive_in_workingslot(void)
{
  return ((CPLD_SLOT_MATRIX_GET() & 0x0f) != 0);
}
#endif // (PTIN_BOARD_IS_LINECARD)

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

/**
 * Get slot position of Active Matrix 
 * (For all cards) 
 *  
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_matrixActive_slot(void)
{
  L7_uint8 slot = 0;
  L7_uint8 working_slot, protection_slot;

 #if (PTIN_BOARD_IS_LINECARD)
  L7_BOOL  olt1t1_backplane;

  /* Condition for OLT1T1 backplane */
  olt1t1_backplane = (ptin_fpga_board_get() == PTIN_BOARD_CXO160G);

  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = (olt1t1_backplane) ? PTIN_SYS_OLT1T1_SLOTS_MAX : PTIN_SYS_OLT1T3_SLOTS_MAX;

  slot = ((CPLD_SLOT_MATRIX_GET() & 0x0f) == 0) ? protection_slot : working_slot;

 #elif (PTIN_BOARD_IS_MATRIX)
  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = PTIN_SYS_MX2_SLOT;

  /* Working slot */
  if (CPLD_SLOT_ID_GET() == 0)
  {
    slot = (CPLD_SLOT_MATRIX_GET() == 0) ? protection_slot : working_slot;
  }
  /* Protection slot */
  else
  {
    slot = (CPLD_SLOT_MATRIX_GET() == 0) ? working_slot : protection_slot;
  }
 #endif

  return slot;
}

/**
 * Get slot position of Inactive Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_matrixInactive_slot(void)
{
  L7_uint8 slot = 0;
  L7_uint8 working_slot, protection_slot;

 #if (PTIN_BOARD_IS_LINECARD)
  L7_BOOL  olt1t1_backplane;

  /* Condition for OLT1T1 backplane */
  olt1t1_backplane = (ptin_fpga_board_get() == PTIN_BOARD_CXO160G);

  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = (olt1t1_backplane) ? PTIN_SYS_OLT1T1_SLOTS_MAX : PTIN_SYS_OLT1T3_SLOTS_MAX;

  slot = ((CPLD_SLOT_MATRIX_GET() & 0x0f) == 0) ? working_slot : protection_slot;

 #elif (PTIN_BOARD_IS_MATRIX)
  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = PTIN_SYS_MX2_SLOT;

  /* Working slot */
  if (CPLD_SLOT_ID_GET() == 0)
  {
    slot = (CPLD_SLOT_MATRIX_GET() == 0) ? working_slot : protection_slot;
  }
  /* Protection slot */
  else
  {
    slot = (CPLD_SLOT_MATRIX_GET() == 0) ? protection_slot : working_slot;
  }
 #endif

  return slot;
}

#endif // (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)


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
* @comments This funcion should be used only for dataplane crossover and not for control crossover
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fpga_mx_get_matrixactive(void)
{
  int current_state = -1;
  static int previous_state = -1;

  if((CPLD_SLOT_MX_ACTIVE_GET() & 0x03) == 0x02) {
      //Master Matrix was set to active
      current_state = PTIN_SLOT_WORK;
  }
  else if ((CPLD_SLOT_MX_ACTIVE_GET() & 0x03) == 0x01) {
      // Slave Matrix was set to active
      current_state = PTIN_SLOT_PROT;
  }
  else
  {
      current_state = previous_state;
  }

  previous_state = current_state;

#if ( PTIN_BOARD_IS_SWITCHABLE )
  /* Note: the register 18h is not affected by the backplane, however lanes/data plane is inverted */
  if (current_state >= 0 &&
      ptin_fpga_board_get() == PTIN_BOARD_CXO160G /*olt1t1_backplane*/)
  {
      return !current_state;
  }
#endif

  return current_state;
}

/**
 * Get active/backup matrix slot id.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup) 
 * 
 * @return slotId  : Slot Id of the  matrix type
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the slotId returned always belongs to the active matrix 
 */
L7_uint8 ptin_fpga_matrix_slotid_get(ptin_fpga_matrix_type_t matrixType)
{
 #if PTIN_BOARD_IS_MATRIX  
  if(matrixType == PTIN_FPGA_STANDBY_MATRIX)  //Return backup matrix slot ID 
  {
    return (ptin_fpga_matrixInactive_slot());
  }
  else //Return active matrix slot ID
  {
    return (ptin_fpga_matrixActive_slot());   
  }
 #elif PTIN_BOARD_IS_LINECARD
  return (ptin_fpga_matrixActive_slot());
 #endif
}

/**
 * Get active/backup matrix IP address.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup)
 * @param 
 * 
 * @return ipAddr    : IP address of the matrix type
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the IP address returned always belongs to the active matrix 
 */
L7_uint32 ptin_fpga_matrix_ipaddr_get(ptin_fpga_matrix_type_t matrixType)
{
  if( ptin_fpga_matrix_slotid_get(matrixType) == PTIN_SYS_MX1_SLOT )
  {
    return (IPC_MX_IPADDR_WORKING);
  }
  else
  {
    return (IPC_MX_IPADDR_PROTECTION);
  }
}

/**
 * Get Board Type
 * 
 * 
 * @return boardType    : PTIN_BOARD_CXO160G 
 *                        PTIN_BOARD_CXO640G 
 *  
 */
L7_uint32 ptin_fpga_board_get(void)
{
  L7_uint32 __board_type; 

  #if PTIN_BOARD_IS_MATRIX
    #if ( PTIN_BOARD == PTIN_BOARD_CXO160G )
      __board_type = PTIN_BOARD_CXO160G; 
    #elif( PTIN_BOARD == PTIN_BOARD_CXO640G )             
      __board_type = PTIN_BOARD_CXO640G;
    #endif
  #elif (PTIN_BOARD_IS_LINECARD)
    /* Condition for OLT1T1 backplane */ 
   if(((CPLD_SLOT_MATRIX_GET() >> 4) & 0x0f) != (CPLD_SLOT_MATRIX_GET() & 0x0f))
   {
     __board_type = PTIN_BOARD_CXO160G;
   }
   else
   {
     __board_type = PTIN_BOARD_CXO640G;
   }
  #else
    #error "Not Supported Yet"
  #endif
  return __board_type;
}

L7_uint32 ptin_fpga_slot_ip_addr_return(L7_uint8 slotId)
{
  L7_uint32 ipAddr;
  ptin_fpga_slot_ip_addr_get(slotId, &ipAddr);
  return ipAddr;
}

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
L7_RC_t ptin_fpga_slot_ip_addr_get(L7_uint8 slotId, L7_uint32 *ipAddr)
{
  L7_BOOL  olt1t1_backplane = L7_FALSE;
  L7_uint8 max_slots;

  olt1t1_backplane = (ptin_fpga_board_get() == PTIN_BOARD_CXO160G);
    
  /*Get the max slots of this system*/
  max_slots = (olt1t1_backplane) ? PTIN_SYS_OLT1T1_SLOTS_MAX : PTIN_SYS_OLT1T3_SLOTS_MAX;

  /*Validate Input Arguments*/
  if (slotId == 0 || slotId > max_slots || ipAddr == L7_NULLPTR)
  {
    /*Invalid Arguments*/
    return L7_FAILURE;
  }

  if (olt1t1_backplane)
  {    
    switch (slotId)
    {
    case 1:/*MX1*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x01;     
      break;
    case  2:/*LC1*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x05;     
      break;
    case 3:/*LC2*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x04;     
      break;
    case 4:/*LC3*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x03;     
      break;
    case 5:/*MX2*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x02;     
      break;    
    default:      
      return L7_FAILURE;
    }
  }
  else /*olt1t3_backplane*/
  {
    switch (slotId)
    {
    case 1:/*MX1*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x01;     
      break;
    case 2: /*LC1*/      
    case 3: /*LC2*/    
    case 4: /*LC3*/      
    case 5: /*LC4*/
    case 6: /*LC5*/
    case 7: /*LC6*/
    case 8: /*LC7*/
    case 9: /*LC8*/
    case 10:/*LC9*/
    case 11:/*LC10*/
    case 12:/*LC11*/
    case 13:/*LC12*/
    case 14:/*LC13*/
    case 15:/*LC14*/
    case 16:/*LC15*/
    case 17:/*LC16*/
    case 18:/*LC17*/
    case 19:/*LC18*/
      *ipAddr = PTIN_IPC_SUBNET_ID | ((slotId+1) & 0x000000FF);    
      break;
    case 20:/*MX2*/
      *ipAddr = PTIN_IPC_SUBNET_ID | 0x02;     
      break;    
    default:      
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
#endif // (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

/**
 * Get slot id 
 * (For all cards)
 * 
 * @return L7_uint8 : slot id
 */
L7_uint8 ptin_fpga_board_slot_get(void)
{
  L7_uint8 board_slot_id = 0;

  board_slot_id = CPLD_SLOT_ID_GET();

#if (PTIN_BOARD_IS_LINECARD)
  {
    L7_BOOL  olt1t1_backplane;

    /* Condition for OLT1T1 backplane */
    olt1t1_backplane = (ptin_fpga_board_get() == PTIN_BOARD_CXO160G);

    /* If high and low nibbles are equal, we are at a OLT1T3 system */
    if (!olt1t1_backplane)
    {
    board_slot_id += 2;
    }
    /* Otherwise, we are at a OLT1T1 system */
    else
    {
    /* Validate slot id */       
    if (board_slot_id > 4)
      return ((L7_uint8) -1);
    /* Invert slot ids */ 
    board_slot_id = 4 - board_slot_id;        
    }
  }
#elif (PTIN_BOARD_IS_MATRIX)
  board_slot_id = (board_slot_id == 0) ? PTIN_SYS_MX1_SLOT : PTIN_SYS_MX2_SLOT;
#endif

  return board_slot_id;
}
