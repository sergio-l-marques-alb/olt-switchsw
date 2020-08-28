
#ifndef GLOBAL_MACRO_H
#define GLOBAL_MACRO_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
/*#include <stdbool.h>*/

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
/* SOFT_DRIVER is used for SV paltform */
/* SV_TEST is used for SV paltform */
#if 0
#define SOFT_DRIVER
#endif

#define OS_LINUX


#ifdef SOFT_DRIVER
#define SV_TEST
#endif


#define MASK_1_BIT            (0x00000001)
#define MASK_2_BIT            (0x00000003)
#define MASK_3_BIT            (0x00000007)
#define MASK_4_BIT            (0x0000000F)
#define MASK_5_BIT            (0x0000001F)
#define MASK_6_BIT            (0x0000003F)
#define MASK_7_BIT            (0x0000007F)
#define MASK_8_BIT            (0x000000FF)
#define MASK_9_BIT            (0x000001FF)
#define MASK_10_BIT            (0x000003FF)
#define MASK_11_BIT            (0x000007FF)
#define MASK_12_BIT            (0x00000FFF)
#define MASK_13_BIT            (0x00001FFF)
#define MASK_14_BIT            (0x00003FFF)
#define MASK_15_BIT            (0x00007FFF)
#define MASK_16_BIT            (0x0000FFFF)
#define MASK_17_BIT            (0x0001FFFF)
#define MASK_18_BIT            (0x0003FFFF)
#define MASK_19_BIT            (0x0007FFFF)
#define MASK_20_BIT            (0x000FFFFF)
#define MASK_21_BIT            (0x001FFFFF)
#define MASK_22_BIT            (0x003FFFFF)
#define MASK_23_BIT            (0x007FFFFF)
#define MASK_24_BIT            (0x00FFFFFF)
#define MASK_25_BIT            (0x01FFFFFF)
#define MASK_26_BIT            (0x03FFFFFF)
#define MASK_27_BIT            (0x07FFFFFF)
#define MASK_28_BIT            (0x0FFFFFFF)
#define MASK_29_BIT            (0x1FFFFFFF)
#define MASK_30_BIT            (0x3FFFFFFF)
#define MASK_31_BIT            (0x7FFFFFFF)
#define MASK_32_BIT            (0xFFFFFFFF)

#define SHIFT_BIT_0            (0)
#define SHIFT_BIT_1            (1)
#define SHIFT_BIT_2            (2)
#define SHIFT_BIT_3            (3)
#define SHIFT_BIT_4            (4)
#define SHIFT_BIT_5            (5)
#define SHIFT_BIT_6            (6)
#define SHIFT_BIT_7            (7)
#define SHIFT_BIT_8            (8)
#define SHIFT_BIT_9            (9)
#define SHIFT_BIT_10        (10)
#define SHIFT_BIT_11        (11)
#define SHIFT_BIT_12        (12)
#define SHIFT_BIT_13        (13)
#define SHIFT_BIT_14        (14)
#define SHIFT_BIT_15        (15)
#define SHIFT_BIT_16        (16)
#define SHIFT_BIT_17        (17)
#define SHIFT_BIT_18        (18)
#define SHIFT_BIT_19        (19)
#define SHIFT_BIT_20        (20)
#define SHIFT_BIT_21        (21)
#define SHIFT_BIT_22        (22)
#define SHIFT_BIT_23        (23)
#define SHIFT_BIT_24        (24)
#define SHIFT_BIT_25        (25)
#define SHIFT_BIT_26        (26)
#define SHIFT_BIT_27        (27)
#define SHIFT_BIT_28        (28)
#define SHIFT_BIT_29        (29)
#define SHIFT_BIT_30        (30)
#define SHIFT_BIT_31        (31)
#define SHIFT_BIT_32        (32)
#define SHIFT_BIT_48        (48)


#define CHECK_BUSY_DELAY_TIME (40)
#define CHECK_BUSY_DELAY_STEP (5)


#define RET_FAIL        (1)
#define RET_SUCCESS     (0)
#define RET_PARAERR      (2)

#define STR_BUF_SIZE    (1000)
#define    CFG_MAXARGS        (600)


#define SHOW_LEVEL_A    (0)
#define SHOW_LEVEL_1    (1)
#define SHOW_LEVEL_2    (2)
#define SHOW_LEVEL_3    (3)
#define SHOW_LEVEL_4    (4)
#define SHOW_LEVEL_5    (5)
#define SHOW_LEVEL_6    (6)


/* define bool type for compile environment */
#define bool int
#define false (0)
#define true  (1)

/* max chip index*/
#define MAX_DEV            (DEV_NUM-1)
#define MAX_CHANNEL        (199)

/* 100G MODE or 50G MODE*/
#define MODE_100G (1)
#define MODE_50G (0)

/* define flexe chip num */
#define DEV_NUM (3)

#define TRAFFIC_ADD    (1)  /* add traffic */
#define TRAFFIC_REMOVE (0)  /* delete traffic */

#define DEFAULT_VALUE   (0xff)   /* define default value */
#define REG_WIDTH       (32)     /* register width */

/* define tcycle */
#define TCYCLE      (10)
#define CH_NUM      (80)
#define TS_NUM      (80)
#define PHY_NUM     (8)
#define MAX_GROUP    (8)
#define CLIENT_NUM  (8)
#define PHY_100G_PL (20)
#define PHY_50G_PL  (10)
#define CH_FIELD    ((CH_NUM / REG_WIDTH) + 1)
#define TS_FIELD    ((TS_NUM / REG_WIDTH) + 1)


/* sar channel num */
#define SAR_CHANNEL_NUM (80)
#define SAR_TS_NUM      (80)

/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/



/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/
#define MAX_INDEX(a) (sizeof(a)/sizeof(a[0]) - 1)


#endif
