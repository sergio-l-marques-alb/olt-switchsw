/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   stk.h
*
* @purpose    Format of the STK file header.
*
* @component  CDA
*
* @comments
*
* @create     7/22/2003
*
* @author     atsigler
* @end
*
**********************************************************************/

#ifndef STK_H_INCLUDED
#define STK_H_INCLUDED

/* Fixed tags that help FASTPATH identify the STK files.
*/
#define STK_TAG1 ((unsigned short) 0xaa55)
#define STK_TAG2 ((unsigned long) 0x2288bb66)

/* Operating system types supported in the STK image.
*/
#define STK_OS_VXWORKS	1
#define STK_OS_LINUX	2

typedef struct 
{
	unsigned short crc;
	unsigned short tag1;
	unsigned long tag2;

	unsigned long num_components; /* Number of OPR and tgz files in the STK image (may be 0) */

	unsigned long file_size; /* Total number of bytes in the STK file */

	unsigned char rel;
	unsigned char ver;
	unsigned char maint_level;
	unsigned char build_num;

	unsigned long stk_header_size; /* Number of bytes in the STk header */

	unsigned char reserved[64];    /* Reserved for future use */

} stkFileHeader_t;

/* The OPR file descriptors immediately follow the STK header. 
*/
typedef struct 
{
	unsigned long offset; /* Offset of first byte of the OPR file from the start of STK file. */
	unsigned long target_device;
	unsigned long os;     /* Operating system of this image */
	unsigned char reserved[16]; /* Reserved for future use */

} stkOprFileInfo_t;

/* image header
 * common for stk and opr files
 */

struct common_image_header
{
    unsigned short  crc;
    unsigned short  tag1;
    unsigned long   word2;
    unsigned long   word3;
    unsigned long   word4;
};

/* Max number of images and header size, for sanity checking of 
 * downloads.
 */
#define STK_MAX_IMAGES 10
#define STK_MAX_HEADER_SIZE (sizeof(stkFileHeader_t) + \
			     (STK_MAX_IMAGES)*(sizeof(stkOprFileInfo_t)))
#endif /* STK_H_INCLUDED */
