/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  iplsec.h
*
* @purpose   This file contains formats of IPL file for the LVL7 family of switches.
*
* @component hardware
*
* @comments  Switch image file has the following format:
* @comments	  ----------------------------
* @comments  | File header                |
* @comments  |----------------------------|
* @comments  | Operational Code           |  (Compressed)
* @comments  |----------------------------|
* @comments  | Boot Code                  |  (Not compressed)
* @comments  |----------------------------|
* @comments  | Symbol Table (for devshell)|  (Compressed)
* @comments   ----------------------------
* @create
*
* @author    paulq
*
* @end
*
*********************************************************************/

#ifndef IPLSEC_H_INCLUDED
#define IPLSEC_H_INCLUDED



/* Size of VPD FILENAME for the code image.
*/
#define IPL_VPD_FILENAME_SIZE     256

/* Size of VPD Time for the code image.
*/
#define IPL_VPD_TIME_SIZE    32

/* VPD data for the code image.
**
**  filename is the fully qualified name of this file
** Release number and version number apply to all components of the code image.
*/

struct ipl_vpd
{
  unsigned char    rel;
  unsigned char    ver;
  unsigned char    maint_level;
  unsigned char    build_num;
  char             timestamp[IPL_VPD_TIME_SIZE];
  char             filename[IPL_VPD_FILENAME_SIZE];

  unsigned char    reserved [128]; /* Reserved for future use. */
};

/* Size of VPD data for the code image.
*/
#define IPL_VPD_DATA_SIZE     sizeof (struct ipl_vpd)


/* Each component in the operational code image contains the following
** attributes:
*/
struct ipl_component_attr
{
  unsigned short crc;
  unsigned short pad1;
  unsigned long  component_id;
  unsigned long  version;
  unsigned long  component_size;
  unsigned long  component_offset; /* Location in boot file after end of header */
  unsigned long  target_address;   /* Optional field to tell the switch where to load this component.*/
  unsigned long  compressed;       /* TRUE - Component is compressed. FALSE - Component is not compressed. */
  unsigned long  store_in_flash;   /* TRUE - Store this component in switch FLASH. FALSE - Do not save in FLASH. */
  unsigned char  pad2[16];
};

/* Maximum Number of components in the code image.
*/
#define IPL_MAX_COMPONENTS	11



/*
* Image header.  Make sure size of ipl_image_header struct is
* a multiple of 4 bytes
*/
typedef struct ipl_image_header
{
  unsigned short crc;  /* two-byte CRC  */
  unsigned short pad1;
  unsigned long target_device; /* Equal to IPL_MODEL_TAG */
  unsigned long size;          /* Image size (includes the header) */
  unsigned long num_components; /* Equal to IPL_NUM_COMPONENTS */
  struct    ipl_component_attr component[IPL_MAX_COMPONENTS];
  unsigned char               copyright[240];
  struct   ipl_vpd            vpd;

  unsigned long writeProtSize;  /* size of sections to write protect */

  /* Minimum version of boot code required to support this release */
  unsigned long minBootVersion; 
#ifdef LVL7_DNI8347
  /* Unfortunately this product shipped initially with a different */
  /* size, so it must be preserved to maintain compatibility.      */
  unsigned char         pad2[12];
#else
  unsigned char         pad2[8];
#endif
} oprHeader_t;

/*
* Header size.
*/
#define IMAGE_HEADER_SIZE	sizeof (struct ipl_image_header)


/* Number of components  */
#define IPL_NUM_COMPONENTS 3


/* Indices into 'component' array */
#define OPCODE			0
#define BOOTCODE		1
#define SYMBOLS                 2

/*
* Operational code is compressed and is stored in the switch FLASH.
*/
#define IPL_OPCODE_ID		       10
#define IPL_OPCODE_VERSION	   1
#define IPL_OPCODE_COMPRESS	   1
#define IPL_OPCODE_LZMA            2
#define IPL_OPCODE_FLASH	     1


/*
* Boot code is not compressed and is not stored in the switch FLASH.
*/
#define IPL_BOOTCODE_ID		    20
#define IPL_BOOTCODE_VERSION	01
#define IPL_BOOTCODE_COMPRESS	0

/*
* Symbols are compressed, and are stored in switch FLASH.
*/
#define IPL_SYMBOLS_ID         30
#define IPL_SYMBOLS_VERSION    01
#define IPL_SYMBOLS_COMPRESS   1
#define IPL_SYMBOLS_FLASH      1
#endif
