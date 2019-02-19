/* $Id: chip_sim_hw_simulator.c,v 1.9 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
/* includes */
#include <stdio.h>
#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <malloc.h>
  #include <wsipx.h>
  #include <fcntl.h>
#elif __VXWORKS__
  #include <vxWorks.h>
  #include <sockLib.h>
  #include <inetLib.h>
  #include <stdioLib.h>
  #include <strLib.h>
  #include <ioLib.h>
  #include <fioLib.h>
#elif DUNE_CSR_CYG
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/time.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <time.h>
  #include <string.h>
  #include <arpa/inet.h>
#endif

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/petra_general.h>

/* $Id: chip_sim_hw_simulator.c,v 1.9 Broadcom SDK $
#include <errno.h>
*/
#define DEFAULT_BUFLEN   1024
#define DEFAULT_SEND_LEN 128
#define DEFAULT_PORT 9999

typedef enum
{
  CHIP_SIM_MSG_TYPE_SET_REG,
  CHIP_SIM_MSG_TYPE_GET_REG,
  CHIP_SIM_MSG_TYPE_SET_MEM,
  CHIP_SIM_MSG_TYPE_GET_MEM,
  CHIP_SIM_MSG_TYPE_SEND_PACKET,
  /*
   *  Number of types in CHIP_SIM_MSG_TYPE
   */
  CHIP_SIM_MSG_NOF_TYPES
}CHIP_SIM_MSG_TYPE;

#define CHIP_SIM_ERR_AND_EXIT(msg) \
  if (res != SOC_SAND_OK)\
  {\
    soc_sand_os_printf(msg);\
    goto exit;\
  }

#define CHIP_SIM_SPRINTF_TO_BUFFER1(args) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args);
#define CHIP_SIM_SPRINTF_TO_BUFFER2(args, args2) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args, args2);
#define CHIP_SIM_SPRINTF_TO_BUFFER3(args, args2, args3) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args, args2, args3);
#define CHIP_SIM_SPRINTF_TO_BUFFER4(args, args2, args3, args4) \
  Chip_sim_buffer_level += sal_sprintf(&(Chip_sim_buffer[Chip_sim_buffer_level]), args, args2, args3, args4);


/*
 *	Global socket definition
 */
#if NEGEV_NATIVE
SOCKET 
#else
int
#endif
  Chip_sim_connect_socket;

static
  uint32
    Chip_sim_buffer_level = 0;

static
  char
    *Chip_sim_buffer;

#define CHIP_SIM_BUFFER_MAX_SIZE               (10000000)

/*
 *	Allocation of place
 */
uint32
  chip_sim_buffer_alloc(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PETRA_ALLOC_ANY_SIZE(Chip_sim_buffer, char, CHIP_SIM_BUFFER_MAX_SIZE);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in chip_sim_buffer_alloc()",0,0);
}
void
  chip_sim_buffer_free(void)
{
  SOC_PETRA_FREE_ANY_SIZE(Chip_sim_buffer);
  Chip_sim_buffer_level = 0;
}

/*
 *	Add the CLI name to the file
 */
uint32
  chip_sim_cli_name_add(
    SOC_SAND_IN  uint32       driver_api_result
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    error_word,
    proc_id ;
  char
    *soc_sand_proc_name,
    *soc_sand_module_name ;

  if (Chip_sim_buffer != NULL)
  {
  /*
   * Initialize error word
   */
    soc_sand_initialize_error_word(0,0,&error_word) ;
    soc_sand_set_proc_id_into_error_word(driver_api_result, &error_word) ;
    proc_id = soc_sand_get_proc_id_from_error_word(error_word) ;
    soc_sand_proc_id_to_string((uint32)proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;
    if ((soc_sand_proc_name[0] == 'P') && (soc_sand_proc_name[1] == 'B'))
    {
      CHIP_SIM_SPRINTF_TO_BUFFER2("CLI: %s", soc_sand_proc_name);
    }
  }
  
  return res;
}

      
/*
 *	Print the file in an output
 */
uint32
  chip_sim_print_output(void)
{
  uint32 
    res = SOC_SAND_OK;
  FILE
    *ofp;
  char
    filename[200] = "";
  uint32
    nof_bytes = 0;
  uint32
    seconds, 
    nano_seconds;

  soc_sand_os_get_time(&seconds, &nano_seconds);
  filename[0] = 0;
  nof_bytes += sal_sprintf(&(filename[nof_bytes]),"C:\\Boot\\simulator\\");
  nof_bytes += sal_sprintf(&(filename[nof_bytes]),"reg_cmd_%d", ((unsigned int) (nano_seconds ^ seconds)) % 100000);
  nof_bytes += sal_sprintf(&(filename[nof_bytes]),".txt");
  ofp = fopen(filename, "w");
  fprintf(ofp, "%s", Chip_sim_buffer);
  fclose(ofp);
  chip_sim_buffer_free();
  res = chip_sim_buffer_alloc();
  goto exit;

exit:
  return res;
}

STATIC
  SOC_SAND_RET
    chip_sim_module_from_addr(
       SOC_SAND_IN  uint32  addr,
       SOC_SAND_OUT uint32  *module_id
    )
{
  *module_id = SOC_PB_NOF_MODULES;

  if ((addr >= 0x6000) && (addr < 0x6400))
  {
    *module_id = SOC_PB_IHP_ID;
  }

  if ((addr >= 0x6400) && (addr < 0x6700))
  {
    *module_id = SOC_PB_IHB_ID;
  }

  if ((addr >= 0x5800) && (addr < 0x5b00))
  {
    *module_id = SOC_PB_EGQ_ID;
  }

  if ((addr >= 0x3a00) && (addr < 0x3c00))
  {
    *module_id = SOC_PB_EPNI_ID;
  }

  return 0;
}



STATIC
  SOC_SAND_RET
    chip_sim_module_to_string(
       SOC_SAND_IN  uint32  module_id,
       SOC_SAND_OUT char *module
    )
{
  SOC_SAND_RET
    res = SOC_SAND_OK;

  switch(module_id) 
  {
  case SOC_PB_OLP_ID:
    sal_strncpy(module, "OLP", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IRE_ID:
    sal_strncpy(module, "IRE", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IDR_ID:
    sal_strncpy(module, "IDR", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IRR_ID:
    sal_strncpy(module, "IRR", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IHP_ID:
    sal_strncpy(module, "IHP", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_QDR_ID:
    sal_strncpy(module, "QDR", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IPS_ID:
    sal_strncpy(module, "IPS", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IPT_ID:
    sal_strncpy(module, "IPT", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_DPI_A_ID:
    sal_strncpy(module, "DPI_A", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_DPI_B_ID:
    sal_strncpy(module, "DPI_B", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_DPI_C_ID:
    sal_strncpy(module, "DPI_C", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_DPI_D_ID:
    sal_strncpy(module, "DPI_D", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_DPI_E_ID:
    sal_strncpy(module, "DPI_E", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_DPI_F_ID:
    sal_strncpy(module, "DPI_F", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_RTP_ID:
    sal_strncpy(module, "RTP", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_EGQ_ID:
    sal_strncpy(module, "EGQ", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_SCH_ID:
    sal_strncpy(module, "SCH", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_CFC_ID:
    sal_strncpy(module, "CFC", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_EPNI_ID:
    sal_strncpy(module, "EPNI", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IQM_ID:
    sal_strncpy(module, "IQM", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_MMU_ID:
    sal_strncpy(module, "MMU", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_IHB_ID:
    sal_strncpy(module, "IHB", DEFAULT_BUFLEN);    
    break;
  case SOC_PB_FDT_ID:
    sal_strncpy(module, "FDT", DEFAULT_BUFLEN);    
    break;

  default:
    res = SOC_SAND_ERR;
    goto exit;
  }

exit:
  return res;
}

STATIC
  SOC_SAND_RET
    chip_sim_buffer_write(
     SOC_SAND_IN  uint32     addr,
     SOC_SAND_IN  uint32     *val,
     SOC_SAND_IN  uint32          nof_vals,
     SOC_SAND_IN  CHIP_SIM_MSG_TYPE  msg_type,
     SOC_SAND_IN  SOC_PB_MODULE_ID      module_id
   )
{
  uint32
    value = val[0],
#if NEGEV_NATIVE
    b_size = 0,
#endif
    buffer_size = 0,
    addr_local=0,
    buffer_size_mem = 0;
  SOC_SAND_RET
    res = SOC_SAND_OK;
  uint32
    mem_ndx,
    val_ndx;
  char
    module[DEFAULT_BUFLEN],
#if NEGEV_NATIVE
    buff_mem[DEFAULT_BUFLEN] = "",
    buffer[DEFAULT_BUFLEN] = "";
#else
    buff_mem[DEFAULT_BUFLEN] = "";
#endif
  char
    buff[DEFAULT_BUFLEN] = "";

  /*
   *	First command
   */
  res = chip_sim_module_to_string(
          module_id,
          module
        );
  if (res != SOC_SAND_OK)
  {
    goto exit;
  }

  if (module_id == SOC_PB_IHP_ID) 
  {
    addr_local = addr - 0x6000;
  } 
  else if (module_id == SOC_PB_IHB_ID) 
  {
    addr_local = addr - 0x6400;
  } 
  else if (module_id == SOC_PB_EGQ_ID) 
  {
    addr_local = addr - 0x5800;
  } 
  else if (module_id == SOC_PB_EPNI_ID) 
  {
    addr_local = addr - 0x3a00;
  } 

  switch(msg_type) 
  {
  case CHIP_SIM_MSG_TYPE_SET_REG:
    /*
     *	Correct the trigger discount (not in ChipSim)
     */
    if ((addr_local == 0x40) && ((value % 2) == 1))
    {
      if (SOC_SAND_GET_BITS_RANGE(value, 15, 2) > 0)
      {
        value = value - 0x4; /*Simulate a discount nof-lines*/
      }
    }
    /*
     *	Only one register each time
     */
    CHIP_SIM_SPRINTF_TO_BUFFER2("$display(\"nof_bytes: %d\");\n\r", Chip_sim_buffer_level);
    CHIP_SIM_SPRINTF_TO_BUFFER4("`%s_REGFILE.write(32'h%04x,32'h%x);\n\r", module, addr_local, value);
    /*
     *	Special case for trigger set
     */
    if ((addr_local == 0x40) && ((value % 2) == 1))
    {
      CHIP_SIM_SPRINTF_TO_BUFFER1("begin\n\r");
      CHIP_SIM_SPRINTF_TO_BUFFER1("  int trigger_counter = 0;\n\r");
      CHIP_SIM_SPRINTF_TO_BUFFER3("  `%s_REGFILE.read(32'h%04x,data);\n\r", module, addr_local);
      CHIP_SIM_SPRINTF_TO_BUFFER1("  while ( (data[0]==1) && (trigger_counter< 32769) ) begin\n\r");
      CHIP_SIM_SPRINTF_TO_BUFFER3("    `%s_REGFILE.read(32'h%04x,data);\n\r", module, addr_local);
      CHIP_SIM_SPRINTF_TO_BUFFER1("    trigger_counter ++;\n\r");
      CHIP_SIM_SPRINTF_TO_BUFFER1("  end\n\r");
      CHIP_SIM_SPRINTF_TO_BUFFER1("  if ( trigger_counter >= 32769 )\n\r");
      CHIP_SIM_SPRINTF_TO_BUFFER2("      `dn_fatal (`IHP_ENV.log, $psprintf(\"Timeout at indirect write - Block [%s], Address [0x000E0001]\"));\n\r", module);
      CHIP_SIM_SPRINTF_TO_BUFFER1("end\n\r");
    }

    break;

  case CHIP_SIM_MSG_TYPE_SET_MEM:
    /*
     *	Build for a potential multi-long table
     */
    for (val_ndx = 0; val_ndx < nof_vals; ++val_ndx)
    {
      mem_ndx = nof_vals - val_ndx - 1;
      buffer_size_mem += sal_sprintf(&(buff_mem[buffer_size_mem]), "%x", val[mem_ndx]);
    }


    buffer_size += sal_sprintf(&(buff[buffer_size]), "set_mem %s %x %s\n\r", module, addr, buff_mem);
    break;

  case CHIP_SIM_MSG_TYPE_SEND_PACKET:
    buffer_size += sal_sprintf(&(buff[buffer_size]), "flow ITMH 1\n\r");
    break;
  
  default:
    res = SOC_SAND_ERR;
    goto exit;
 }

exit:
  return res;
}


unsigned short
  soc_sand_eci_hw_write(
    const uint32 *array,
    uint32       *base_address,
    const uint32 offset,
    const uint32 size
 )
{
  uint32
    ui,
    local_size;
  uint32 
    module_id, 
    res = SOC_SAND_OK;
  CHIP_SIM_MSG_TYPE  
    msg_type = CHIP_SIM_MSG_TYPE_SET_REG;

  /*
   *	Write also in the ChipSim as usual
   */
  res = soc_sand_eci_write(
          array,
          base_address,
          offset,
          size
        );



  local_size    = size   >> 2 ;
  for (ui=0 ; ui<local_size ; ++ui)
  {
    res = chip_sim_module_from_addr(
            offset / sizeof(long) + ui,
            &module_id
          );

    res = chip_sim_buffer_write(
            offset / sizeof(long) + ui,
            array,
            0,
            msg_type,
            module_id
          );

    array++;
  }


  return 0 ;
}

uint32
  hw_client_activate(void)
{
  SOC_SAND_PHYSICAL_ACCESS
    hw_access;

  hw_access.physical_write = soc_sand_eci_hw_write;
  hw_access.physical_read = soc_sand_eci_read;

  soc_sand_set_physical_access_hook(&hw_access);

  return 0;
}

uint32
  hw_client_close(void)
{
#if NEGEV_NATIVE
  closesocket(Chip_sim_connect_socket);
#else
  soc_sand_os_printf("hw_client_close\n\r");
#endif

  return SOC_SAND_OK;
}
