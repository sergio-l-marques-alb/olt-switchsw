#ifndef _PTIN_FPGA_OLT1T0_H
#define _PTIN_FPGA_OLT1T0_H

#include "cpld_spi.h"

/* FPGA AND CPLD BASE ADDRESS */
//# define SPI_FPGA
# define SPI_CPLD

/* PLD map registers */
# define CPLD_ID                       0x1255
# define CPLD_ID0_REG                  0x0000
# define CPLD_ID1_REG                  0x0001
# define CPLD_VER_REG                  0x0002
# define CPLD_BUILD_REG                0x0003
# define CPLD_HW_ID_REG                0x0008
# define CPLD_HW_VER_REG               0x0009
# define CPLD_CHASSIS_ID_REG           0x000C
# define CPLD_CHASSIS_VER_REG          0x000D
# define CPLD_SLOT_ID_REG              0x000F
# define CPLD_MX_ACTIVE_REG            0x0018
# define CPLD_SLOT_MATRIX_REG          0x001E

/* FPGA map registers */
# define FPGA_ID                       0x1256
# define FPGA_ID0_REG                  0x0000
# define FPGA_ID1_REG                  0x0001
# define FPGA_VER_REG                  0x0002


#ifdef SPI_CPLD
#define CPLD_INIT()               cpld_spi_create()
#define CPLD_DEINIT()             cpld_spi_destroy()
#define CPLD_REG_GET(addr)        cpld_spi_read(addr)
#define CPLD_REG_SET(addr, val)   cpld_spi_write(addr, val)
#define CPLD_ID_GET()             (((unsigned short) CPLD_REG_GET(CPLD_ID0_REG)<<8) | ((unsigned short) CPLD_REG_GET(CPLD_ID1_REG)))
#define CPLD_SLOT_ID_GET()        CPLD_REG_GET(CPLD_SLOT_ID_REG)
#define CPLD_SLOT_MATRIX_GET()    CPLD_REG_GET(CPLD_SLOT_MATRIX_REG)
#define CPLD_SLOT_MX_ACTIVE_GET() CPLD_REG_GET(CPLD_MX_ACTIVE_REG)
#else
#define CPLD_INIT()
#define CPLD_DEINIT()
#define CPLD_REG_GET(addr)        0
#define CPLD_REG_SET(addr, val)
#define CPLD_ID_GET()             0
#define CPLD_SLOT_ID_GET()        0
#define CPLD_SLOT_MATRIX_GET()    1
#define CPLD_SLOT_MX_ACTIVE_GET() 0
#endif


#define FPGA_INIT()
#define FPGA_DEINIT()
#define FPGA_REG_GET(addr)        0
#define FPGA_REG_SET(addr, val)
#define FPGA_ID_GET()             0

#endif /* _PTIN_FPGA_OLT1T0_H */

