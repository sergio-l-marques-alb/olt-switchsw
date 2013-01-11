/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mk_opr.c
*
* @purpose   This file creates an .opr file from the switch's 
# @purpose   operational code and bootrom by computing, their CRC, 
* @purpose   and attaching the transmission header.                                                             
*
* @component hw
*
* @create    01/18/2000
*
* @author    paulq 
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h> /* htonl(), ntohl() */
#include <sys/stat.h>

#include "../../../../../src/system_support/base/l7util/zlib_win/zlib.h"
#include "iplsec.h"
#include "platform_cpu.h"

#define  INCLUDE_LVL7_BOOTCODE

/* Global Variables
*/
/* extern errno; */

short calc_crc(FILE *fp);
short calc_buf_crc(char *buf, unsigned long bufsize);
short calc_bootcode_checksum( FILE *fp);
int filesize(FILE *fp);

static char    *progname;      /* my name */

char usage[]="Usage in preferred order of parameters:  \n%s -t <target> -r <rel> -v <ver> [-m <maint>]  [-b <bld>]\n"\
             "where\n"\
             "<target> (required) is target type, a char string < 80 chars in length\n"\
             "<rel>    (required) is Release Number, any numeric value (0-63) or 1 alpha character\n"\
             "<ver>    (required) is Version Number, any numeric value (0-63)\n"\
             "<maint>  (optional) is Maintanence Level, any numeric value (0-63)\n"\
             "<bld>    (optional) is Build Number, any numeric value (0-63)\n";

char copyright_string[]= "(C) Copyright Broadcom Corporation 2003-2009 All Rights Reserved. ";

static char relbuf[10];
/*****************************************************************************
** NAME: parse_cmd_line
**
** Read parameters passed to the program.
**
*****************************************************************************/
static void parse_cmd_line(int argc, char *argv[], 
                           char *targ, 
                           unsigned char *rel, 
                           unsigned char *ver,
                           unsigned char *maint_lvl,
                           unsigned char *bnum,                       
                           char *imagefile)
{
  char **av = argv;
  char *tailptr;
  int i;
  unsigned long rc;
  char c;
  int got_target=0;
  int got_rel=0;
  int got_ver=0;
  int got_maint=0;
  int got_bnum=0;                                          
  int tempInt;
  char buf[10];                                            

  progname = argv[0];

  if ( argc < 7 )
  {
    fprintf(stderr,usage,progname);
    exit (-1);
  }
  argc--, av++;
  while ( argc > 0 && *av[0] == '-' )
  {
    while ( argc > 0 && *++av[0] )
    {
      switch ( *av[0] )
      {
        case 't':
          argc--, av++;
          if (strlen(av[0]) > 80)
          {
            printf("Invalid length for target - length must be < 80\n\n");
            exit ( 0 );
          }
          strcpy (targ,av[0]);
          argc--;
          av++;
          got_target=1;
          break;

        case 'r':
          argc--, av++;
          if ( isalpha(*av[0]) != 0 )
          {
            /* Alpha character... */
            if (strlen(av[0]) > 1)
            {
              printf("\n\nRelease string is too long. It must be only 1 character.\n\n");
              exit(0);
            }
            strcpy (relbuf,av[0]);
            rel[0] = relbuf[0];
          }
          else
          {
            /* Digit character... */
            tempInt = strtoul(av[0], &tailptr, 10);
            if ( tempInt > 63 )
            {
              printf("Invalid value for Release - value must between 0 and 63\n\n");
              exit ( 0 );
            }
            *rel = tempInt;
            sprintf(relbuf, "%d", tempInt);
          }
          argc--, av++;
          got_rel=1;
          break;

        case 'v':
          argc--, av++;
          if ( isalpha(*av[0]) != 0 )
          {
            printf("Invalid value for Version - value must be numeric\n\n");
            exit ( 0 );
          }
          tempInt = strtoul(av[0], &tailptr, 10);
          if ( tempInt > 63 )
          {
            printf("Invalid value for Version - value must between 0 and 63\n\n");
            exit ( 0 );
          }
          *ver = tempInt;
          argc--, av++;
          got_ver=1;
          break;

        case 'b':                                                          
          argc--, av++;
          if ( isalpha(*av[0]) != 0 )
          {
            printf("Invalid value for Build Number - value must be numeric\n\n");
            exit ( 0 );
          }
          tempInt = strtoul(av[0], &tailptr, 10);
          if ( tempInt > 63 )
          {
            printf("Invalid value for Build Number - value must between 0 and 63\n\n");
            exit ( 0 );
          }
          if (tempInt == 0)
          {
            break;
          }
          *bnum = tempInt;
          argc--, av++;
          got_bnum=1;
          break;                                                           

        case 'm':
          argc--, av++;
          if ( isalpha(*av[0]) != 0 )
          {
            printf("Invalid value for Maintanence Level - value must be numeric\n\n");
            exit ( 0 );
          }
          tempInt = strtoul(av[0], &tailptr, 10);
          if ( tempInt > 63 )
          {
            printf("Invalid value for Maintanence Level - value must between 0 and 63\n\n");
            exit ( 0 );
          }
          *maint_lvl = tempInt;
          argc--, av++;
          got_maint=1;
          break;

        default:
          fprintf(stderr,usage,progname);
          exit(-1);
          break;
      }
      /* argc--, av++; */
    }
  }

  if ( (got_target == 1) && (got_rel == 1) && (got_ver == 1) )
  {
    sprintf(imagefile,"%sr%sv%u",targ,relbuf,*ver);
    if ( got_maint == 1 )
    {
      sprintf(buf,"m%u",*maint_lvl);
      strcat(imagefile,buf);
      if ( got_bnum == 1 )
      {
        sprintf(buf,"b%u",*bnum);
        strcat(imagefile,buf);
      }
      else
      {
        *bnum=0;
      }
    }
    else
    {
      *maint_lvl=0;
      if ( got_bnum == 1 )
      {
        sprintf(buf,"m%ub%u",*maint_lvl,*bnum);
        strcat(imagefile,buf);
      }
      else
      {
        *bnum=0;
      }
    }
    strcat(imagefile,".opr");
  }
  else
  {
    fprintf(stderr,usage,progname);
    exit(-1);
  }
  return;
}


/***************************************************************************
**
**  MODULE : crc16.c
**                     
**  FUNCTION :        
** This file contains routines to perform byte-at-a-time CRC computation.
**
** The program is derived from pascal program written by Donald R. Blake.
**                                                                  
**  CHANGE SUMMARY :                                               
**    date   SID  Init    Action                                  
**   ----------------------------                                
**   22oct93 1.1  ALT     Created file.                         
**                                                            
***************************************************************************/
#define POLYNOMIAL 0xA097  /* Use 0x8005 for CRC16, 0x1021 for SDLC/HDLC */

static unsigned short CrcTable[256]; /* CRC lookup table */

/************************************************************************
** NAME: UpdateCRC
**
** This function updates existing CRC 
**
** INPUT:
**	DataByte - Received data byte
**	
** INPUT/OUTPUT:
**	crc - Current value of CRC.
************************************************************************/
void UpdateCRC (unsigned char DataByte, unsigned short *crc)
{
  register unsigned long new_crc;

  new_crc = *crc;
  new_crc = (new_crc << 8) ^ (CrcTable[new_crc >> 8] ^ DataByte);
  *crc = (unsigned short) new_crc;
};


/************************************************************************
** NAME: GenCrcTable
**
** This function computes CRC look-up table.
************************************************************************/
void GenCrcTable (void)
{
  unsigned short CrcCode;
  unsigned char DataByte;
  unsigned long  index, BitNo;
  unsigned long  CarryBit, DataBit;

  for ( index = 0; index <= 255; index++ )
  {
    CrcCode = 0;
    DataByte = (unsigned char)index;

    for ( BitNo = 1; BitNo <= 8; BitNo++ )
    {
      CarryBit = ((CrcCode & 0x8000) != 0);
      DataBit = ((DataByte & 0x80) != 0);
      CrcCode = (unsigned short)(CrcCode << 1);
      if ( CarryBit ^ DataBit )
      {
        CrcCode = (unsigned short)(CrcCode ^ POLYNOMIAL);
      }
      DataByte = (unsigned char)(DataByte << 1);
    }; /* End For */
    CrcTable [index] = CrcCode;
  }; /* End for */
};

/****************************************************************************
** NAME: create_transmission_header
**
** Create transmission header.
**
** INPUT:
**  rel    - Release number
**  ver    - Version number.
**  maint_lvl - Maintenance number.
**  build_num - Build number for this Maintenance release.
**
** COMMENTS:
** The filename format is as follows: Release-Version-MaintLevel-BuildNumber.
** BuildNumber is used only internally to track the builds for a particular release.
** A release will only consist of Release-Version-MaintLevel.
** In order to not have to change the ipl_vpd structure to match this new layout
** of the fields, which would require a reflash of all boxes, that structure is left 
** unchanged. So there is a mismatch of nomenclature but the field locations remain the same.
****************************************************************************/
static void create_transmission_header (char  rel,
                                        char  ver,
                                        char  maint_lvl,
                                        char  build_num,                                    
                                        char *image_file,
                                        unsigned long opcode_size,
                                        unsigned long bootcode_size,
					unsigned long symbols_size,
                                        unsigned short opcode_crc,
                                        unsigned short bootcode_crc,
					unsigned short symbols_crc)
{
  struct ipl_image_header  image_header;
  int num_items;
  FILE * ipl_image_file, * temp_file;
  int i,j,rc;
  int num_read,num_written;
  unsigned short  crc;
  time_t t;
  int c;
  char cmd[128];
  unsigned int num_components = 1;
  unsigned long current_offset = 0;

  bzero (&image_header, sizeof (image_header));

  time(&t);
  /* 
  * Create Image header.
  */
  image_header.pad1           = 0;
  image_header.target_device  = htonl(IPL_MODEL_TAG);
  image_header.vpd.rel        = rel;
  image_header.vpd.ver        = ver;
  image_header.vpd.maint_level = maint_lvl;
  image_header.vpd.build_num  = build_num;                            
#ifdef BOOTCODE_MIN_VERSION
  image_header.minBootVersion = htonl(BOOTCODE_MIN_VERSION);
#endif

  strncpy(image_header.vpd.timestamp, ctime(&t), sizeof(image_header.vpd.timestamp)-1);
  strncpy(image_header.vpd.filename, image_file, sizeof(image_header.vpd.filename)-1);

  image_header.component[OPCODE].crc              = htons(opcode_crc);
  image_header.component[OPCODE].component_id     = htonl(IPL_OPCODE_ID);
  image_header.component[OPCODE].version          = htonl(IPL_OPCODE_VERSION);
  image_header.component[OPCODE].component_size   = htonl(opcode_size);
  image_header.component[OPCODE].component_offset = htonl(current_offset);
  image_header.component[OPCODE].target_address   = htonl(IPL_OPCODE_TARGET);
  image_header.component[OPCODE].compressed       = htonl(IPL_OPCODE_LZMA);
  image_header.component[OPCODE].store_in_flash   = htonl(IPL_OPCODE_FLASH);
  current_offset += opcode_size; 
  if ( bootcode_size )
  {
    num_components++;
    image_header.component[BOOTCODE].crc              = htons(bootcode_crc);
    image_header.component[BOOTCODE].component_id     = htonl(IPL_BOOTCODE_ID);
    image_header.component[BOOTCODE].version          = htonl(IPL_BOOTCODE_VERSION);
    image_header.component[BOOTCODE].component_size   = htonl(bootcode_size);
    image_header.component[BOOTCODE].component_offset = htonl(current_offset);
    image_header.component[BOOTCODE].target_address   = htonl(IPL_BOOTCODE_TARGET);
    image_header.component[BOOTCODE].compressed       = htonl(IPL_BOOTCODE_COMPRESS);
    image_header.component[BOOTCODE].store_in_flash   = htonl(IPL_BOOTCODE_FLASH);
    current_offset += bootcode_size;
  }
  if ( symbols_size )
  {
    num_components++;
    image_header.component[SYMBOLS].crc               = htons(symbols_crc);
    image_header.component[SYMBOLS].component_id      = htonl(IPL_SYMBOLS_ID);
    image_header.component[SYMBOLS].version           = htonl(IPL_SYMBOLS_VERSION);
    image_header.component[SYMBOLS].component_size    = htonl(symbols_size);
    image_header.component[SYMBOLS].component_offset  = htonl(current_offset);
    image_header.component[SYMBOLS].target_address    = htonl(0);
    image_header.component[SYMBOLS].compressed        = htonl(IPL_SYMBOLS_COMPRESS);
    image_header.component[SYMBOLS].store_in_flash    = htonl(IPL_SYMBOLS_FLASH);
  }
  
  image_header.num_components = htonl(num_components);         
  image_header.size = htonl(opcode_size + bootcode_size + symbols_size + IMAGE_HEADER_SIZE);

  strncpy(image_header.copyright, copyright_string, sizeof(image_header.copyright)-1);

  /* 
  * Create IPL Image file. Old file, if present, is deleted.
  */
  ipl_image_file = fopen (image_file, "wb");
  if ( ipl_image_file == NULL )
  {
    printf("Couldn't open %s\n",image_file);
    exit (-1);
  }
  temp_file=fopen("temp.gz","rb");
  if ( temp_file == NULL )
  {
    printf("Couldn't open temp.gz\n");
    exit (-1);
  }

  /* 
  * Write header into the target file.
  */
  num_items = fwrite (&image_header, IMAGE_HEADER_SIZE, 1, ipl_image_file);
  if ( num_items != 1 )
  {
    printf("Failed while writing header to %s\n",image_file);
    fclose (temp_file);
    unlink("temp.gz");
    exit (-1);
  }
  j=filesize(temp_file);
  if (j < 0)
  {
    printf("Couldn't read temp.gz file size\n");
    fclose (temp_file);
    unlink("temp.gz");
    exit (-1);
  }
  for ( i = 0; i < j; i++ )
    fputc (fgetc(temp_file), ipl_image_file);

  fclose (temp_file);
  unlink ("temp.gz");

  fclose(ipl_image_file);
  ipl_image_file=fopen(image_file,"rb+");
  if ( ipl_image_file == NULL )
  {
    printf("Couldn't open %s\n",image_file);
    exit (-1);
  }

  GenCrcTable ();
  crc = 0;

  /* 
  * Skip the first two bytes of the image. The two bytes contain CRC.
  */
  c = fgetc (ipl_image_file);
  c = fgetc (ipl_image_file);

  do
  {
    c = fgetc (ipl_image_file);
    if ( c != EOF )
    {
      UpdateCRC (c, &crc);
    }
  } while ( c != EOF );
  fclose (ipl_image_file);

  /* 
  * Write CRC to the image file.
  */
  ipl_image_file = fopen (image_file, "rb+");
  if ( ipl_image_file == NULL )
  {
    printf ("ERROR: Can't open %s\n", image_file);
    exit (-1);
  }

  num_items = fread (&image_header, IMAGE_HEADER_SIZE, 1, ipl_image_file);
  if ( num_items != 1 )
  {
    printf ("ERROR: Can't read %s\n", image_file);
    exit (-1);
  }

  rewind (ipl_image_file);
  image_header.crc = htons(crc);

  /* 
  * Following the fread(), ensure header string fields are once again
  * null-terminated prior to displaying them.
  */
  image_header.vpd.timestamp[sizeof(image_header.vpd.timestamp)-1] = '\0';
  image_header.vpd.filename[sizeof(image_header.vpd.filename)-1] = '\0';
  image_header.copyright[sizeof(image_header.copyright)-1] = '\0';

  num_items = fwrite (&image_header, IMAGE_HEADER_SIZE, 1, ipl_image_file);
  if ( num_items != 1 )
  {
    printf ("ERROR: Can't write %s\n", image_file);
    exit (-1);
  }

  fclose (ipl_image_file);

  /* 
  * Display image header information on the build console.
  *
  * NOTE: The ntohl() and ntohs() functions may actually be macros that are
  *       #defined away as nothing.  Using an explicit typecast for each
  *       of these so that the value is always of the expected type to
  *       match the format string.
  */

  printf ("\n\nThe following image header has been created:\n\n"); 

  printf ("CRC............................0x%x (%u)\n", (unsigned short)ntohs(image_header.crc), (unsigned short)ntohs(image_header.crc));
  printf ("Target Device..................0x%x\n", (unsigned int)ntohl(image_header.target_device));
  printf ("Size...........................0x%x (%u)\n", (unsigned int)ntohl(image_header.size), (unsigned int)ntohl(image_header.size));
  printf ("Number of Components...........%u\n\n", (unsigned int)ntohl(image_header.num_components));


  printf ("Operational Code Size..........0x%x (%u)\n", 
          (unsigned int)ntohl(image_header.component[OPCODE].component_size),
          (unsigned int)ntohl(image_header.component[OPCODE].component_size));
  printf ("Operational Code Offset........0x%x (%u)\n",
          (unsigned int)ntohl (image_header.component[OPCODE].component_offset),
          (unsigned int)ntohl (image_header.component[OPCODE].component_offset));
  printf ("Operational Code FLASH flag....%u\n",
          (unsigned int)ntohl (image_header.component[OPCODE].store_in_flash));
  printf ("Operational Code CRC...........0x%X\n\n",
          (unsigned short)ntohs(image_header.component[OPCODE].crc));


  if ( bootcode_size )
  {
    printf ("Boot Code Version..............%u\n",
            (unsigned int)ntohl(image_header.component[BOOTCODE].version));
    printf ("Boot Code Size.................0x%x (%u)\n", 
            (unsigned int)ntohl(image_header.component[BOOTCODE].component_size), 
            (unsigned int)ntohl(image_header.component[BOOTCODE].component_size));
    printf ("Boot Code Offset...............0x%x (%u)\n", 
            (unsigned int)ntohl(image_header.component[BOOTCODE].component_offset), 
            (unsigned int)ntohl(image_header.component[BOOTCODE].component_offset));
    printf ("Boot Code FLASH flag...........%u\n",
            (unsigned int)ntohl (image_header.component[BOOTCODE].store_in_flash));
    printf ("Boot Code CRC..................0x%X\n\n",
            (unsigned short)ntohs(image_header.component[BOOTCODE].crc));
  }

  if ( symbols_size )
  {
    printf ("Symbols Version................%u\n",
            (unsigned int)ntohl(image_header.component[SYMBOLS].version));
    printf ("Symbols Size...................0x%x (%u)\n", 
            (unsigned int)ntohl(image_header.component[SYMBOLS].component_size), 
            (unsigned int)ntohl(image_header.component[SYMBOLS].component_size));
    printf ("Symbols Offset.................0x%x (%u)\n", 
            (unsigned int)ntohl(image_header.component[SYMBOLS].component_offset), 
            (unsigned int)ntohl(image_header.component[SYMBOLS].component_offset));
    printf ("Symbols FLASH flag.............%u\n",
            (unsigned int)ntohl (image_header.component[SYMBOLS].store_in_flash));
    printf ("Symbols CRC....................0x%X\n\n",
            (unsigned short)ntohs(image_header.component[SYMBOLS].crc));
  }

  printf ("VPD............................release %s, ", relbuf);
  printf ("version %u, maint_lvl %u", image_header.vpd.ver, image_header.vpd.maint_level);

  if ( image_header.vpd.build_num )
  {
    printf (", build_num %u\n", image_header.vpd.build_num);
  }
  else
  {
    printf ("\n");
  }

  printf (
          "Timestamp......................%s\n"
          "File...........................%s\n",
          image_header.vpd.timestamp,
          image_header.vpd.filename
          );
  }




main (int argc, char *argv[])
{
  unsigned char rel, ver, maint_lvl, build_num; 
  char target[80], load_image[80];
  unsigned long opcode_size = 0;
  unsigned long bootcode_size = 0;
  unsigned long bootcode_filesize = 0;
  unsigned long startcode_size = 0;
  unsigned long symbols_size = 0;
  unsigned long uncompr_symbols_size = 0;
  int i,j;
  int num_read,num_written;
  unsigned short opcode_crc = 0;
  unsigned short bootcode_crc = 0;
  unsigned short symbols_crc = 0;
  FILE *temp_file;
  FILE *opcode_file;
  FILE *bootcode_file;
  FILE *symbols_file;
  FILE *final_image;
  char *inbuf, *compr_buf;
  unsigned long codeStart;
  unsigned long insize;
  int rc;
  short bootcode_checksum = 0;
  unsigned int checksum;
  char c;
  short *inbuf16;
  int readChar;
  struct stat fileStat;
#define OPCODE_FILE_NAME "bootos.bin.lzma"

  checksum = 0;

  printf("\n\n");
  parse_cmd_line(argc, argv,  
                 &target[0], &rel, &ver, &maint_lvl, &build_num, load_image);

  if (stat(OPCODE_FILE_NAME, &fileStat) != 0)
  {
    printf("Could not stat %s\n", OPCODE_FILE_NAME);
    exit(-1);
  }
  insize = fileStat.st_size;

  opcode_file=fopen(OPCODE_FILE_NAME,"rb");
  if ( opcode_file==NULL )
  {
    printf("Couldn't open bootos.bin\n");
    exit(-1);
  }
  
  inbuf=(char *)malloc(insize);
  if ( inbuf==NULL )
  {
    printf("  ** malloc() error... quitting **\n");
    exit(-1);
  }
  
  for ( i = 0; i < insize; i++ )
  {
    inbuf[i]=fgetc(opcode_file);
  }
    
  fclose(opcode_file);

  opcode_size=insize;
  
  printf("Writing compressed image file...\n");
  /* write compressed data to a file */
  final_image=fopen("temp.gz","wb+");
  if ( final_image==NULL )
  {
    printf("Can't open temp.gz...quitting\n");
    free(inbuf);
    exit(-1);
  }
  for ( i=0;i<insize;i++ )
  {
    fputc(inbuf[i],final_image);
  }

  free(inbuf);

  /* 
  * each component must be a multiple of 4 bytes, so pad if necessary 
  */
  fseek(final_image,0L,SEEK_END);
  j = filesize(final_image);
  if (j < 0)
  {
    printf("Can't get size of temp.gz file...quitting\n");
    fclose(final_image);
    exit(-1);
  }
  j = (4 - j%4)%4;
  for ( i=0;i<j;i++ )
  {
    fputc(0xFF,final_image);
    opcode_size++;
  }

  /* 
  * compute operational code CRC..........
  */
  opcode_crc = calc_crc(final_image); 

  /* 
  *
  * Add boot code. The boot code is optional.
  *
  */
#ifdef INCLUDE_LVL7_BOOTCODE
#define BOOTROM_FILE_NAME "../start/vxboot.opr"
  bootcode_file=fopen(BOOTROM_FILE_NAME,"rb");

  if ( bootcode_file != NULL )
  {
    printf("Processing %s...\n", BOOTROM_FILE_NAME);
    temp_file=fopen("temp.bin","wb+");
    if ( temp_file==NULL )
    {
      printf("Can't create temp.bin\n");
      exit(1);
    }

    readChar = fgetc(bootcode_file);
    while ( readChar != EOF )
    {
      fputc(readChar,temp_file);
      bootcode_size++;
      readChar = fgetc(bootcode_file);
    }

    fclose(bootcode_file);
    /* pad bootcode_file to make it a multiple of 4 bytes */
    j = (4 - bootcode_size%4)%4;
    for ( i=0;i<j;i++ )
    {
      fputc(0xFF,temp_file);
      bootcode_size++;
    }
    rewind(temp_file);

    /* Append boot code to the image. */
    /* Calculate CRC of boot code */
    bootcode_crc = calc_crc(temp_file);

    for ( i = 0; i < bootcode_size; i++ )
    {
      fputc (fgetc(temp_file), final_image);
    }

    fclose (temp_file);
  }
  else
    printf("Could not open %s\n", BOOTROM_FILE_NAME);

#endif

  if (stat("devshell_symbols", &fileStat) != 0)
  {
    printf("Could not stat %s\n", OPCODE_FILE_NAME);
  }
  uncompr_symbols_size = fileStat.st_size;

  symbols_file=fopen("devshell_symbols","rb");

  if ( symbols_file != NULL )
  {
    printf("Processing devshell_symbols...\n");
    inbuf = malloc(uncompr_symbols_size);
    symbols_size = 1.1*uncompr_symbols_size+12; /* Worst case */
    compr_buf = malloc(symbols_size);
    if ((inbuf == NULL) || (compr_buf == NULL))
    {
      printf("Not enough memory to compress devshell symbols\n");
      exit(-1);
    }
    i = 0;
    readChar = fgetc(symbols_file);
    while ( readChar != EOF )
    {
      inbuf[i] = readChar;
      i++;
      readChar = fgetc(symbols_file);
    }
    fclose(symbols_file);
    if ((rc = compress2(compr_buf, &symbols_size, inbuf, uncompr_symbols_size, 9)) != Z_OK) 
    {
      printf("Error %d compressing devshell symbols\n", rc);
      exit(-1);
    }
    free(inbuf);
    /* pad file to make it a multiple of 4 bytes */
    j = (4 - symbols_size%4)%4;
    for ( i=0;i<j;i++ )
    {
      compr_buf[symbols_size] = 0xFF;
      symbols_size++;
    }

    /* Append devshell symbols to the image. */
    /* Calculate CRC */
    symbols_crc = calc_buf_crc(compr_buf, symbols_size);

    for ( i = 0; i < symbols_size; i++ )
    {
      fputc (compr_buf[i], final_image);
    }
    free(compr_buf);
  }
  else
    printf("Could not open devshell_symbols.gz\n");

  fclose(final_image);

  create_transmission_header (rel, 
                              ver, 
                              maint_lvl, 
                              build_num, 
                              load_image,
                              opcode_size,
                              bootcode_size,
			      symbols_size,
                              opcode_crc,
                              bootcode_crc,
			      symbols_crc);
  return(0);

}

/* -------------------------------------------------------------------------- */
/* Find the size of the file in bytes */
int filesize(FILE *fp)
{
  int size;                      /* holds the size of the file */
  long org_pos = ftell(fp);      /* save the original position */
  if (org_pos < 0)
  {
    return (int)org_pos;         /* return error value from ftell */
  }
  fseek(fp,0L,SEEK_END);         /* goto the end of the file */
  size=(int)ftell(fp);           /* find the size of the file */
  fseek(fp,org_pos,SEEK_SET);    /* go back to the original position */
  return size;                   /* return the size of the file */
}
/*---------------------------------------------------------------------------*/
/* Find the crc of a buffer */
short calc_buf_crc(char *buf, unsigned long bufsize)
{
  short crc;
  unsigned long index;

  GenCrcTable ();
  crc = 0;
  for(index=0;index<bufsize;index++)
  {
    UpdateCRC(buf[index], &crc);
  }
  return crc;
}

/* Find the crc of a file */
short calc_crc(FILE *fp)
{
  short crc;
  int c;
  long org_pos=ftell(fp);
  rewind(fp);

  GenCrcTable ();
  crc = 0;

  do
  {
    c = fgetc (fp);
    if ( c != EOF )
    {
      UpdateCRC (c, &crc);
    }
  } while ( c != EOF );

  fseek(fp,org_pos,SEEK_SET);
  return crc;
}
/*---------------------------------------------------------------------------*/

short calc_bootcode_checksum( FILE *fp)
{
  unsigned int checksum;
  char c;
  short *inbuf;
  int file_size;
  int i;


  rewind(fp);
  checksum = 0;

  file_size = filesize(fp);
  if (file_size < 0)
  {
    printf("Error reading bootcode file size\n");
    exit(-1);
  }

  inbuf=(short *)malloc(file_size);

  if (fread(inbuf, 1, file_size, fp) != file_size)
  {
    printf("Error reading bootcode file\n");
    free(inbuf);
    exit(-1);
  }

  for ( i=0;i < file_size/2; i++ )
  {
    checksum += ((inbuf[i] & 0x00FF) << 8) | ((inbuf[i] & 0xFF00) >> 8);
    /* check for carry */
    if ( checksum & 0x00010000 )
    {
      checksum &= 0x0000FFFF;
      checksum++;
    }
  }
  free(inbuf);

  return((short)checksum);
}

