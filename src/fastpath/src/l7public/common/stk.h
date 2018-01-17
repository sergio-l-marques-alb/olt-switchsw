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
#include "datatypes.h"

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
			     
typedef enum
{
  STK_SUCCESS,
  STK_FAILURE,
  STK_IMAGE_DOESNOT_EXIST,
  STK_INVALID_IMAGE,
  STK_FILE_SIZE_FAILURE,
  STK_FILE_SIZE_MISMATCH,
  STK_TOO_MANY_IMAGES_IN_STK,
  STK_STK_EMPTY,
  STK_PLATFORM_MISMATCH,
  STK_INVALID_IMAGE_FORMAT,
  STK_NOT_ACTIVATED,
  STK_ACTIVATED,
  STK_TABLE_IS_FULL,
  STK_VALID_IMAGE,
  STK_LAST_ENTRY
}STK_RC_t;

/*********************************************************************
* @purpose  Get the dim error string associated with the dim code
*
* @param    code      - stk return code
* @param    errBuf    - place to copy the error string
* @param    len       - Maximum number of bytes to copy
*
* @returns  L7_SUCCESS   - if the err string is copied successfully
* @returns  L7_FAILURE   - if the errBuf is NULL or code is invalid
*
* @end
********************************************************************/
L7_RC_t cliUtilGetStkErrorString(STK_RC_t code, L7_char8* errBuf, L7_uint32 len);

#endif /* STK_H_INCLUDED */
