/* AAPL CORE Revision: master
 *
 * Copyright (c) 2014-2021 Avago Technologies. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Internal library header file for AAPL (ASIC and ASSP Programming Layer). */

/** Doxygen File Header
 ** @file
 ** @brief Declarations for library functions.
 **/

#ifndef AAPL_LIBRARY_H_
#define AAPL_LIBRARY_H_

/* Typedefs:
 */
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#if ! HAVE_UINT
typedef unsigned int uint;
#endif

/* Define type for eye gather counts: */
#if defined __STDC_VERSION && __STDC_VERSION__ > 199409L
  typedef long long bigint;
#elif defined __MINGW_EXTENSION
  typedef __int64 bigint;
#elif defined _STDINT_H
  typedef int64_t bigint;
#else
  typedef long long bigint;
#endif

#if defined X86_64
typedef bigint   fn_cast_t;
#else
typedef long int fn_cast_t;
#endif


/* Determine number of elements in a static array: */
#define AAPL_ARRAY_LENGTH(a)   (int)(sizeof(a) / sizeof((a)[0]))
#define AAPL_BITS_SET(var,width,shift,value) var = ((var & ~(((1<<width)-1)<<(shift))) | ((value)<<(shift)))
#define AAPL_BITS_GET(var,width,shift) ((var >> (shift)) & ((1<<width)-1))

/* Specify how to declare a 64 bit constant: */
#ifdef __INT64_C
#   define AAPL_CONST_INT64(x) __INT64_C(x)
#elif defined(__GNUC__) && !defined(__STDC_VERSION__)
#   define AAPL_CONST_INT64(x) ((bigint)x)
#elif defined(__GNUC__) && defined(WIN32)
#   define AAPL_CONST_INT64(x) (x ## LL)
#elif defined(WIN32)
#   define AAPL_CONST_INT64(x) (x ## i64)
#else
#   define AAPL_CONST_INT64(x) (x)
#endif

#ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS

/** @cond INTERNAL */

#if AAPL_ENABLE_MAIN && defined(__linux)
  EXT void display_backtrace(void);
#else
#  define display_backtrace() do{}while(0)
#endif

EXT void  ms_sleep(uint millisec);

/* Converts data to a 32 character ASCII encoded binary str with optional underscores every 8 bits */
EXT char *aapl_hex_2_bin(char *str, uint data, int underscore_en, int bits);

/* Portability utility functions: */
EXT char *aapl_strcasestr(const char *s, const char *find);
EXT int   aapl_strcasecmp(const char *s1, const char *s2);
EXT int   aapl_strncasecmp(const char *s1, const char *s2, size_t len);
EXT char *aapl_strdup(const char *string);
EXT void  aapl_str_rep(char *str, char search, char replace);
EXT char *aapl_strtok_r(char *str, const char *delim, char **saveptr);
EXT size_t aapl_local_strftime(char *buf, size_t max, const char *format);

typedef struct
{
    const char *const name;
    int value;
} Aapl_conv_table_t;

EXT int name_to_index(Aapl_conv_table_t tbl[], const char *name, uint skip_prefix);
EXT int value_to_index(Aapl_conv_table_t tbl[], int value);
/** @endcond */

#endif

EXT const char *aapl_bigint_to_str(bigint value);

/** @brief Aapl_log_type_t is used for the AAPL struct log buffers */
typedef enum
{
    AVAGO_DEBUG0 = 0,
    AVAGO_DEBUG1, AVAGO_DEBUG2, AVAGO_DEBUG3, AVAGO_DEBUG4, AVAGO_DEBUG5,
    AVAGO_DEBUG6, AVAGO_DEBUG7, AVAGO_DEBUG8, AVAGO_DEBUG9,
    AVAGO_DEBUG10, AVAGO_DEBUG11, AVAGO_DEBUG12,
    AVAGO_DEBUG13, AVAGO_DEBUG14, AVAGO_DEBUG15,
    AVAGO_MEM_LOG, AVAGO_DATA_CHAR, AVAGO_DATA_CHAR_ADD,
    AVAGO_ERR, AVAGO_WARNING, AVAGO_INFO
} Aapl_log_type_t;

typedef enum
{
    AVAGO_UNKNOWN_IP                 = 0x00,
    AVAGO_SERDES                     = 0x01,
    AVAGO_SBUS_CONTROLLER            = 0x02,
    AVAGO_SPICO                      = 0x03,
    AVAGO_QPI                        = 0x04,
    AVAGO_BLACKHAWK                  = 0x05,
    AVAGO_PCS64B66B                  = 0x06,
    AVAGO_AUTO_NEGOTIATION           = 0x07,
    AVAGO_PCS64B66B_FEC              = 0x08,
    AVAGO_PCIE_PCS                   = 0x09,
    AVAGO_CORE_PLL                   = 0x0a,
    AVAGO_PMRO                       = 0x0b,
    AVAGO_HBM3_APC                   = 0x0c,
    AVAGO_HBM3_CG                    = 0x0d,
    AVAGO_DDR_ADDRESS                = 0x0c,
    AVAGO_DDR_DATA                   = 0x0d,
    AVAGO_DDR_TRAIN                  = 0x0e,
    AVAGO_DDR_CTC                    = 0x0f,
    AVAGO_DDR_STOP                   = 0x10,
    AVAGO_HBM_CTC                    = 0x0f,
    AVAGO_HBM_STOP                   = 0x10,
    AVAGO_THERMAL_SENSOR             = 0x11,
    AVAGO_RMON                       = 0x12,
    AVAGO_LINK_EMULATOR              = 0x13,
    AVAGO_AVSP_CONTROL_LOGIC         = 0x14,
    AVAGO_M4                         = 0x15,
    AVAGO_P1                         = 0x16,
    AVAGO_MLD                        = 0x17,
    AVAGO_RSFEC_BRIDGE               = 0x18,
    AVAGO_CROSSPOINT                 = 0x19,
    AVAGO_MAX                        = 0x1a,
    AVAGO_SBUS2APB                   = 0x1b,
    AVAGO_EFUSE                      = 0x1d,
    AVAGO_RESCAL                     = 0x1e,
    AVAGO_CPLL_LC                    = 0x1f,
    AVAGO_SAPPH_GBX                  = 0x20,
    AVAGO_SAPPH_GBX_TOP              = 0x21,
    AVAGO_OPAL_RSFEC528              = 0x22,
    AVAGO_OPAL_RSFEC528_544          = 0x23,
    AVAGO_OPAL_HOST_ALIGNER          = 0x24,
    AVAGO_OPAL_MOD_ALIGNER           = 0x25,
    AVAGO_OPAL_CONTROL               = 0x26,
    AVAGO_OSC_SSC_PLL                = 0x27,
    AVAGO_GARNET_25GE_INTERFACE      = 0x28,
    AVAGO_SGMII                      = 0x29,
    AVAGO_APC                        = 0x2a,
    AVAGO_MLD_RX2                    = 0x2b,
    AVAGO_PMRO2                      = 0x2c,
    AVAGO_OSPREY                     = 0x2d,
    AVAGO_RS4K_FEC                   = 0x2e,
    AVAGO_BCH16K_FEC                 = 0x2f,

    AVAGO_ADCTEMP                    = 0x30,
    AVAGO_BIAS                       = 0x31,
    AVAGO_CLKBUF                     = 0x32,
    AVAGO_ELA                        = 0x33,
    AVAGO_MXS_TEST                   = 0x34,
    AVAGO_MXS_SUBSYSTEM              = 0x35,
    AVAGO_PLLSYS                     = 0x36,
    AVAGO_ROSC                       = 0x37,
    AVAGO_XTAL                       = 0x38,
    AVAGO_SBUS2APB32                 = 0x39,
    AVAGO_ADCSAR                     = 0x3a,
    AVAGO_PLLSYS01                   = 0x3b,
    AVAGO_PLLSSC                     = 0x3c,
    AVAGO_ADCSDET                    = 0x3d,
    AVAGO_D5_TT                      = 0x3e,

    AVAGO_5GLTE_A2D                  = 0x40,
    AVAGO_5GLTE_D2A                  = 0x41,
    AVAGO_5GLTE_RX_GB                = 0x42,
    AVAGO_5GLTE_TX_GB                = 0x43,
    AVAGO_5GLTE_CONV_PLL             = 0x44,
    AVAGO_5GLTE_AGC                  = 0x45,
    AVAGO_5GLTE_CAPRAM               = 0x46,
    AVAGO_KEYSTONE_HSIO              = 0x47,
    AVAGO_KEYSTONE_RX_CORE           = 0x48,
    AVAGO_KEYSTONE_TX_CORE           = 0x49,
    AVAGO_KEYSTONE_MISC              = 0x4a,

    AVAGO_CHIPLET_PCS_MEMORY         = 0x4b,



    AVAGO_IP_TYPE_MAX                = 0x4b,

    AVAGO_IP_TYPE_ALT_RANGE_LO       = 0x81,
    AVAGO_LINK_EMULATOR_2            = 0x81,
    AVAGO_SLE_PKT                    = 0x82,
    AVAGO_SLE                        = 0x83,
    AVAGO_MXU                        = 0x84,

    AVAGO_PAD_CONTROL                = 0x88,

    AVAGO_RAM_PMRO,
    AVAGO_PANDORA_LSB,

    AVAGO_VTMON,
    AVAGO_CUSTOMER_IP_0 = 0x90,
    AVAGO_CUSTOMER_IP_1,
    AVAGO_CUSTOMER_IP_2,
    AVAGO_CUSTOMER_IP_3,
    AVAGO_CUSTOMER_IP_4,
    AVAGO_CUSTOMER_IP_5,
    AVAGO_CUSTOMER_IP_6,
    AVAGO_CUSTOMER_IP_7,
    AVAGO_CUSTOMER_IP_8,
    AVAGO_CUSTOMER_IP_9,
    AVAGO_CUSTOMER_IP_A,
    AVAGO_CUSTOMER_IP_B,
    AVAGO_CUSTOMER_IP_C,
    AVAGO_CUSTOMER_IP_D,
    AVAGO_CUSTOMER_IP_E,
    AVAGO_CUSTOMER_IP_F,
    AVAGO_1K_OTP,
    AVAGO_4K_OTP,
    AVAGO_ANCIL,
    AVAGO_NWL,
    AVAGO_D5,
    AVAGO_D5E = AVAGO_D5,
    AVAGO_D7S_SBUS2APB = AVAGO_D5,
    AVAGO_D7S,
    AVAGO_PROM,
    AVAGO_PLDA_CONTROLLER,
    AVAGO_MTIP_PCS_400G,
    AVAGO_MTIP_PCS_800G,
    AVAGO_CDMII2MAX_INIT,
    AVAGO_CDMII2MAX_TARGET,
    AVAGO_IO_PMRO,

    AVAGO_IP_TYPE_ALT_RANGE_HI,


    AVAGO_MAX_RING_ADDRESS           = 0xdf,

    AVAGO_SGMII_BROADCAST            = 0xea,
    AVAGO_OSC_SSC_BROADCAST          = 0xeb,
    AVAGO_MLD_BROADCAST              = 0xec,
    AVAGO_SERDES_P1_BROADCAST        = 0xed,
    AVAGO_SERDES_M4_BROADCAST        = 0xee,
    AVAGO_THERMAL_SENSOR_BROADCAST   = 0xef,

    AVAGO_SBUS2APB32_BROADCAST       = 0xe8,
    AVAGO_5GLTE_A2D_BROADCAST        = 0xe9,
    AVAGO_5GLTE_D2A_BROADCAST        = 0xed,
    AVAGO_5GLTE_RX_GB_BROADCAST      = 0xee,
    AVAGO_5GLTE_TX_GB_BROADCAST      = 0xef,

    AVAGO_DDR_STOP_BROADCAST         = 0xf0,
    AVAGO_HBM_STOP_BROADCAST         = 0xf0,
    AVAGO_DDR_CTC_BROADCAST          = 0xf1,
    AVAGO_HBM_CTC_BROADCAST          = 0xf1,
    AVAGO_DDR_TRAIN_BROADCAST        = 0xf2,
    AVAGO_APC_BROADCAST              = 0xf2,
    AVAGO_DDR_DATA_BROADCAST         = 0xf3,
    AVAGO_MAX_BROADCAST              = 0xf3,
    AVAGO_HBM3_CG_BROADCAST          = 0xf3,
    AVAGO_DDR_ADDRESS_BROADCAST      = 0xf4,
    AVAGO_RESCAL_ADDRESS_BROADCAST   = 0xf4,
    AVAGO_HBM3_APC_BROADCAST         = 0xf4,

    AVAGO_PMRO_BROADCAST             = 0xf5,
    AVAGO_PMRO2_BROADCAST            = 0xf5,
    AVAGO_RESERVED_BROADCAST         = 0xf6,
    AVAGO_PCIE_PCS_BROADCAST         = 0xf7,
    AVAGO_MXU_BROADCAST              = 0xf8,
    AVAGO_PCS64B66B_BROADCAST        = 0xf8,
    AVAGO_AUTO_NEGOTIATION_BROADCAST = 0xf9,
    AVAGO_OSPREY_BROADCAST           = 0xfa,
    AVAGO_BLACKHAWK_BROADCAST        = 0xfb,
    AVAGO_QPI_BROADCAST              = 0xfc,
    AVAGO_MLD_RX2_BROADCAST          = 0xfc,
    AVAGO_SPICO_BROADCAST            = 0xfd,
    AVAGO_SERDES_D6_BROADCAST        = 0xff,
    AVAGO_SERDES_BROADCAST           = 0xff
} Avago_ip_type_t;

/** @addtogroup Address
 ** @{
 **/

/** @brief AAPL addressing constants. */
typedef enum
{
    AVAGO_ADDR_BROADCAST      = 0xff,
    AVAGO_ADDR_IGNORE_LANE    = 0xf0,
    AVAGO_ADDR_PAIR_0         = 0x20,
    AVAGO_ADDR_PAIR_1         = 0x21,
    AVAGO_ADDR_PAIR_2         = 0x22,
    AVAGO_ADDR_PAIR_3         = 0x23,
    AVAGO_ADDR_QUAD_LOW       = 0x40,
    AVAGO_ADDR_QUAD_HIGH      = 0x41,
    AVAGO_ADDR_QUAD_ALL       = 0xff,
    AVAGO_ADDR_SINGLE_LANE    = 0xfe
} Aapl_broadcast_control_t;
/** @} */

#endif

