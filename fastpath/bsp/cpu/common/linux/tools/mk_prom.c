/**************************************************************************
*
* This tool builds the VPD files used by FASTPATH to identify hardware
* configurations and other parameters.
*
**************************************************************************/

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h> /* htonl(), ntohl() */
#include <sys/stat.h>
#include "l7_common.h"
#include "cfd.h"


extern void GenCrcTable(void);
extern void UpdateCRC(unsigned char, unsigned short *);

/* Global Variables
*/
unsigned char get_choice( void );
static void print_mac (unsigned char * mac_address);
static int  make_mac (char * input_mac, unsigned char * mac);
short calc_crc(FILE *fp);
void base_eeprom (void);


static char    *progname;     /* my name */

char usage[]             ="\n\nUsage:  \n%s\n";

char lvl7_copyright[]            = "(C) Copyright Broadcom Corporation (2003-2005) All Rights Reserved. ";

char password[8] = "debug";

void check_size( int current_size, int capacity )
{
  if ( current_size > capacity )
    printf("Over capacity! Currently at %d size! Capacity is %d\n", current_size, capacity);
}

/**************************************************************************
*
* @purpose  Entry Point routine
*
* @param    command line arguments 
*
* @returns  none
*
* @end
*
*************************************************************************/
int main (int argc, char *argv[])
{
  int c;

  progname = argv[0];

  if ( argc > 1 )
  {
    fprintf(stderr,usage,progname);
    exit (-1);
  }
  base_eeprom ();
};

/**************************************************************************
*
* @purpose  Find the crc of a file 
*
* @param    fp FILE pointer of buffer to calculate CRC 
*
* @returns  calculated CRC
*
* @end
*
*************************************************************************/
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
    };
  } while ( c != EOF );

  fseek(fp,org_pos,SEEK_SET);
  return(crc);
}


/**************************************************************************
*
* @purpose  This function lets the user choose a menu option.  
*
* @param    none
*
* @returns  choice
*
* @end
*
*************************************************************************/
unsigned char get_choice()
{
  char string[81];
  char choice[81];
  int i;
  int j=0;

  bzero(string,sizeof(string));
  bzero(choice,sizeof(choice));
  fgets(string, sizeof(string), stdin);
  if (string[strlen(string)-1] == '\n')
    string[strlen(string)-1] = '\0'; /* remove newline */
  for ( i=0;i<strlen(string);i++ )
  {
    if ( string[i]==0x08 && j>0 )
    { /*backspace*/
      j--;
    }
    else
    {
      choice[j++]=string[i];
    }
  }
  return(atoi(choice));
}


/**************************************************************************
*
* @purpose  This function lets the user choose a menu option.  
*
* @param    none
*
* @returns  choice
*
* @end
*
*************************************************************************/
static int  make_mac (char * input_mac, unsigned char * mac)
{
  int i;
  unsigned char c;
  unsigned char temp_mac [12];

  for ( i = 0; i < 12; i++ )
  {
    c = input_mac [i];
    if ( (c >= '0') && (c <= '9') )
      temp_mac[i] = c - '0';
    else if ( (c >= 'a') && (c <= 'f') )
      temp_mac[i] = c - 'a' + 10;
    else if ( (c >= 'A') && (c <= 'F') )
      temp_mac[i] = c - 'A' + 10;
    else return(1);   /* Invalid character */
  };

  for ( i = 0; i < 12; i+= 2 )
  {
    mac[i/2] = (temp_mac[i] * 16) + temp_mac[i+1];
  };
  return(0);
};

/**************************************************************************
*
* @purpose  This routine prints the mac address. 
*
* @param    mac_address ptr to mac to print
*
* @returns  none
*
* @end
*
*************************************************************************/
static void print_mac (unsigned char * mac_address)
{
  int i;

  for ( i = 0; i < 6; i++ )
  {
    printf ("%02X", mac_address[i]);
    if ( i < 5 ) printf (":");
    else printf (" ");
  };
};

/**************************************************************************
*
* @purpose  Create the Base Card's CPU Serial EEPROM
*
* @param    none
*
* @returns  choice
*
* @comments The user is prompted for several parameters
*
* @end
*
*************************************************************************/
void base_eeprom(void)
{
  int i, j, numberOfMacs, numberOfproms, len;
  char mac_address_input[21], 
  number_of_proms[21],
  pn[21],
  serial_number_input[21],
  maint_input[13],
  orig_serial_number[21],
  eepromBuffer[SERIAL_EEPROM_SIZE];
  unsigned char startingMacAddress[7];
  unsigned long bottomMacAddress;  /* contains the lowest 4 bytes of the mac address for arithmetic */
  unsigned short crc;
  FILE *out_file;
  int rc;
  char tmpname[14];
  int tmpfd;
  char cmd[256];

  /*
  *
  * Get the number of proms to build....
  *
  */
  printf("\n\nEnter the number of Configuration files you wish to create.\n->"); 
  fgets (number_of_proms, sizeof(number_of_proms), stdin);
  if (number_of_proms[strlen(number_of_proms)-1] == '\n')
    number_of_proms[strlen(number_of_proms)-1] = '\0'; /* remove newline */
  numberOfproms = atoi( number_of_proms );
  /*
  *
  * Request the starting MAC address for this build....
  *
  */
  do
  {
    fflush(0);
    printf("\n\nEnter the six byte starting MAC address (example: 000629328140)	\n->");
    fgets (mac_address_input, sizeof(mac_address_input), stdin);
    if (mac_address_input[strlen(mac_address_input)-1] == '\n')
      mac_address_input[strlen(mac_address_input)-1] = '\0'; /* remove newline */
    if ( strlen (mac_address_input) != 12 )
    {
      printf ("ERROR: Invalid MAC address length.\n\n");
      continue;
    }
    if ( make_mac (mac_address_input, startingMacAddress) )
    {
      printf ("ERROR: Invalid characters in the MAC address.\n\n");
      continue;
    }
    printf ("Entered MAC: ");
    print_mac (startingMacAddress);
    printf ("\n");
    bottomMacAddress = 0;
    bottomMacAddress += ( startingMacAddress[2] << 24 );
    bottomMacAddress += ( startingMacAddress[3] << 16 );
    bottomMacAddress += ( startingMacAddress[4] <<  8 );
    bottomMacAddress +=   startingMacAddress[5];
    break;
  } while ( 1 );

  /*
  *
  * Request the maintanence level for this build....
  *
  */
  do
  {
    printf("\n\nEnter the maintanence level for this build (alphanumeric) \n->");
    fgets (maint_input, sizeof(maint_input), stdin);
    if (maint_input[strlen(maint_input)-1] == '\n')
      maint_input[strlen(maint_input)-1] = '\0'; /* remove newline */
    if ( strlen(maint_input)>8 )
    {
      printf("Maint Number must be 8 characters or less in length.\n\n");
      continue;
    }
    break;
  } while ( 1 );

  /*
  *
  * Request the starting serial number for this build....
  *
  */
  do
  {
    printf("\n\nEnter the starting Serial Number for this build (alphanumeric) \n->");
    fgets (serial_number_input, sizeof(serial_number_input), stdin);
    if (serial_number_input[strlen(serial_number_input)-1] == '\n')
      serial_number_input[strlen(serial_number_input)-1] = '\0'; /* remove newline */
    if ( strlen(serial_number_input)>20 )
    {
      printf("Serial Number must be 20 characters or less in length.\n");
      continue;
    }
    strcpy(orig_serial_number,serial_number_input);
    break;
  } while ( 1 );

  /*
  *
  * Set the number of MAC addresses each Base NVRAM is to reserve....
  *
  */
  numberOfMacs = 0x80;

  /*
  *
  * Now generate the prom files based on the user's input....
  *
  */
  for ( i=0;i<numberOfproms;i++ )
  {
    memset ( eepromBuffer, 0, SERIAL_EEPROM_SIZE );
    j=0;


    eepromBuffer[j++] = 0; /* CRC bytes */
    eepromBuffer[j++] = 0;

    eepromBuffer[j++] = 8; 
    eepromBuffer[j++] = CFD_CARD_BASE_MAC;
    eepromBuffer[j++] = startingMacAddress[0];
    eepromBuffer[j++] = startingMacAddress[1]; 
    eepromBuffer[j++] = ( char )( ( bottomMacAddress & 0xFF000000 ) >>24 );
    eepromBuffer[j++] = ( char )( ( bottomMacAddress & 0x00FF0000 ) >>16 ); 
    eepromBuffer[j++] = ( char )( ( bottomMacAddress & 0x0000FF00 ) >>8 );
    eepromBuffer[j++] = ( char )(  bottomMacAddress & 0x000000FF ) ;

    eepromBuffer[j++] = 3;
    eepromBuffer[j++] = CFD_NUMBER_MAC_ADDRESSES;
    eepromBuffer[j++] = numberOfMacs;

    check_size(j,SERIAL_EEPROM_SIZE );

    eepromBuffer[j++] = strlen( serial_number_input ) + 3 ; /* plus 3 for type field and length (max 64 bytes)  + NULL */
    eepromBuffer[j++] = CFD_SN; 
    strcpy(&eepromBuffer[j], serial_number_input);
    j += strlen( serial_number_input );
    eepromBuffer[j++] = 0;
    check_size(j,SERIAL_EEPROM_SIZE );

    eepromBuffer[j++] = CFD_MAINT_LVL;
    strcpy(&eepromBuffer[j],maint_input );
    j += strlen( maint_input );
    eepromBuffer[j++] = 0;
    check_size(j,SERIAL_EEPROM_SIZE );

    eepromBuffer[j] = 0xB; /* type field + length field + 8 char password + NULL */
    eepromBuffer[j+1] = CFD_PASSWORD;
    strcpy(&eepromBuffer[j+2],password );

    eepromBuffer[j+10] = 0;
    j+=11;
    check_size(j,SERIAL_EEPROM_SIZE );

    GenCrcTable ();
    crc = 0;
    for ( j = 2; j < SERIAL_EEPROM_SIZE; j++ )
    {
      UpdateCRC (eepromBuffer[j], &crc);
    }
    eepromBuffer[0] = (crc & 0xff00) >> 8;
    eepromBuffer[1] = crc & 0xff;

    /*
    *
    * Now generate the file name and write this EEPROM file
    *
    */
    out_file=fopen(serial_number_input,"wb+");
    if ( out_file==NULL )
    {
      printf("Couldn't open %s\n", serial_number_input );
      exit(-1);
    }
    printf ("Writing file %s..\n", serial_number_input );

    rc = fwrite (&eepromBuffer,         /* where to copy from */
                 1,                      /* element size */
                 SERIAL_EEPROM_SIZE,        /* no. of elements */
                 out_file );             /* stream to write to */
    fclose (out_file);

    /*
    *
    * Increase the base MAC address for the next card/eeprom
    *
    */
    bottomMacAddress += numberOfMacs;

    /*
    *
    * Increase to the next serial number...
    *
    */
    len = strlen( serial_number_input ) - 1;

    /*  Add 1 to the lowest 4 digits */
    if ( serial_number_input[len] == '9' )
    {
      serial_number_input[len] = '0';
      if ( serial_number_input[len-1] == '9' )
      {
        serial_number_input[len-1] = '0';
        if ( serial_number_input[len-2] == '9' )
        {
          serial_number_input[len-2] = '0';
          serial_number_input[len-3] += 1;
        }
        else
          serial_number_input[len-2] += 1;
      }
      else
        serial_number_input[len-1] += 1; 
    }
    else
      serial_number_input[len] += 1;
  }
  printf("-----------------------------------\nSummary:\n");
  printf("	1) The starting base MAC address for this build -> "); 
  print_mac (startingMacAddress); printf("\n");
  printf("	2) The number of MAC addresses per Base         -> %d\n", numberOfMacs); 
  printf("	3) The starting serial number                   -> %s\n", orig_serial_number); 
  printf("	4) The number of Configuration files built      -> %d\n", numberOfproms); 
  printf("-----------------------------------\n\n");
}
