#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h> /* htonl(), ntohl() */
#include <sys/stat.h>

static char    *progname;     /* my name */

char usage[]                     ="\n\nUsage:  \n%s \\fully qualified path to bootos.map\\\n";
char lvl7_copyright[]            = "(C) Copyright Broadcom Corporation (2003-2005) All Rights Reserved. ";

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
main (int argc, char *argv[])
{


  int i,j,c;
  char *tok;             /* an initString token */
  char *holder = NULL;   /* points to initString fragment beyond tok */
  FILE *bootos_map_file, *mem_layout_file;
  int rc;
  char line[512], templine[512], path[1024];
  unsigned int text_start, text_size;
  unsigned int rodata_start, rodata_size;
  unsigned int data_start, data_size;
  unsigned int bss_start, bss_size;
  int text_found, data_found, rodata_found, bss_found;
  char **av = argv;

  progname = argv[0];

  if ( argc < 2 )
  {
    fprintf(stderr,usage,progname);
    exit (-1);
  }

  av++;
  sprintf(path, "%s\\bootos.map", av[0]); 
  bootos_map_file = fopen(path,"r");
  if ( bootos_map_file == NULL )
  {
    printf("Couldn't open %s\\bootos.map\n", av[0]);
    exit(-1);
  }

  i = 0;

  text_found = data_found = rodata_found = bss_found = 0;
  do
  {
    if (fgets(line, (size_t)sizeof(line), bootos_map_file) == NULL)
      break;

    strcpy(templine, line);
    tok = (char *)strtok_r (templine, " ", &holder);
    if ((strcmp(tok, ".text") == 0) && (text_found == 0))
    {
      text_found = 1;
      tok = (char *)strtok_r (NULL, " ", &holder);
      text_start = (unsigned int) strtoul (tok, NULL, 16);

      tok = (char *)strtok_r (NULL, " ", &holder);
      text_size = (unsigned int) strtoul (tok, NULL, 16);
      
    }

    if ((strcmp(tok, ".rodata") == 0) && (rodata_found == 0))
    {
      rodata_found = 1;
      tok = (char *)strtok_r (NULL, " ", &holder);
      rodata_start = (unsigned int) strtoul (tok, NULL, 16);

      tok = (char *)strtok_r (NULL, " ", &holder);
      rodata_size = (unsigned int) strtoul (tok, NULL, 16);
      
    }



    if ((strcmp(tok, ".data") == 0) && (data_found == 0))
    {
      data_found = 1;
      tok = (char *)strtok_r (NULL, " ", &holder);
      data_start = (unsigned int) strtoul (tok, NULL, 16);

      tok = (char *)strtok_r (NULL, " ", &holder);
      data_size = (unsigned int) strtoul (tok, NULL, 16);
      
    }

    if ((strcmp(tok, ".bss") == 0) && (bss_found == 0))
    {
      bss_found = 1;
      tok = (char *)strtok_r (NULL, " ", &holder);
      bss_start = (unsigned int) strtoul (tok, NULL, 16);

      tok = (char *)strtok_r (NULL, " ", &holder);
      bss_size = (unsigned int) strtoul (tok, NULL, 16);

    }
  } while(1);


  printf("\n\n");

  printf("         top +----------------------------+ 0x%08x (%d)\n", 
         bss_size + data_size + rodata_size + text_size + text_start,
         bss_size + data_size + rodata_size + text_size + text_start);
  for (i=0;i < bss_size/0x40000;i++)
  {
    if (i == (bss_size/0x40000)/2 )
    {
      printf("             | Section Size = 0x%08x  |\n", bss_size);
    }
    else
    {
      printf("             |                            |\n");
    }
  }
  printf("   bss Start |----------------------------| 0x%08x \n", bss_start);
  for (i=0;i < data_size/0x40000;i++)
  {
    if (i == (data_size/0x40000)/2 )
    {
      printf("             | Section Size = 0x%08x  |\n", data_size);
    }
    else
    {
      printf("             |                            |\n");
    }
  }
  printf("  data Start |----------------------------| 0x%08x \n", data_start);

  for (i=0;i < rodata_size/0x40000;i++)
  {
    if (i == (rodata_size/0x40000)/2 )
    {
      printf("             | Section Size = 0x%08x  |\n", rodata_size);
    }
    else
    {
      printf("             |                            |\n");
    }
  }
  printf("rodata Start |----------------------------| 0x%08x \n", rodata_start);
  for (i=0;i < text_size/0x40000;i++)
  {
    if (i == (text_size/0x40000)/2 )
    {
      printf("             | Section Size = 0x%08x  |\n", text_size);
    }
    else
    {
      printf("             |                            |\n");
    }
  }
  printf("  Text Start +----------------------------+ 0x%08x \n\n\n", text_start);





  fclose (bootos_map_file);
  exit(0);

}
