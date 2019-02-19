#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
//#include <asm/page.h>		/* PTin removed */

#include "image.h"
#include "stk.h"

#define PAGE_SIZE (getpagesize())	/* PTin added */

/* --------------------------------------------------------------------- */

#define MAX_IMAGES 24
int extract=0;
int display=0;

stkFileHeader_t stkHdr;
stkOprFileInfo_t oprHdr;

image_header_t imageHdr;
unsigned int imageSizes[MAX_IMAGES];
int imageCount = 0;


int open_input(char * file) {
  int fd;
  int cnt = 0;
  unsigned long size;

  fd=open(file,O_RDONLY);
  if(fd==-1) {
    printf("error: cannot open input file %s\n",file);
    exit(2);
  };

  /* Read the image header */
  read(fd, &stkHdr, sizeof(stkFileHeader_t));

  if ((ntohs(stkHdr.tag1) == STK_TAG1) &&
     (ntohl(stkHdr.tag2) == STK_TAG2))
  {
    read(fd, &oprHdr, sizeof(stkOprFileInfo_t));

  } else {
    printf("warning: invalid STK header, assuming uimage\n");
    lseek(fd, 0, 0);
  }

  /* Read the image header */
  read(fd, &imageHdr, sizeof(image_header_t));

  if (ntohl(imageHdr.ih_magic) != IH_MAGIC) {
    printf("error: invalid uimage header\n");
    exit(2);
  }

  read(fd, &size, sizeof(unsigned long));
  while ((size != 0) && (cnt < MAX_IMAGES)) {
    /* Ensure image sizes are even */
    imageSizes[cnt] = ntohl(size);
    cnt++;
    read(fd, &size, sizeof(unsigned long));
  }
  imageCount = cnt;
  if (cnt == MAX_IMAGES) {
    printf("warning: maximum number of supported images reached\n"); 
  }

  return(fd);
}

int open_output(char * file) {
    int fd;

  if (strcmp(file, "-") == 0) {
    return 1; /* stdout */
  }

  fd=open(file,O_CREAT | O_RDWR, S_IRWXU);
  if(fd==-1) {
    printf("error: cannot open output file %s\n",file);
    exit(2);
  };
  return(fd);
}

#define READ_BUFFER_SIZE 4096
int extract_image(reqImageNum, inFd, outFd)
{
   int offset = 0;
   int count;
   char buffer[READ_BUFFER_SIZE];
   int readCount;
   int bytesToRead;
   int cur_pos;
   void *src_addr, *copy_src;
   int pages, write_rc;
   struct stat st;

   /* Compute image start address */
   for (count = 0; count < reqImageNum; count++) {
      /* Compensate for observed bug in mkimage with odd sized file */
      /* The output file is padded with a byte thus aligning it on  */
      /* an even boundary, but the size does not reflect the the    */
      /* padded byte.                                               */
      offset+= (imageSizes[count] + 3) & 0xfffffffc;
   }

   cur_pos = lseek(inFd, offset, 1);
   bytesToRead =  imageSizes[reqImageNum];
   if (fstat(inFd, &st) < 0) {
     perror("Can't figure out how big the file is");
     exit(1);
   }
   if ((offset > st.st_size) || ((cur_pos + bytesToRead) > st.st_size)) {
     fprintf(stderr, "Corrupt data: Image claims to extend past end of file\n");
     exit(1);
   }

   /* Extract image */
   pages = ((bytesToRead + cur_pos) / PAGE_SIZE) + 1;
   src_addr = mmap(NULL, pages*PAGE_SIZE, PROT_READ, MAP_SHARED, inFd, 0);
   if (src_addr == MAP_FAILED) {
     perror("Could not map memory for copy");
     exit(1);
   }
   copy_src = (void *)(((char *)src_addr) + cur_pos);
   
   do {
     write_rc = write(outFd, copy_src, bytesToRead);
     if (write_rc < 0) {
       perror("Error on write");
       exit(1);
     }
     bytesToRead -= write_rc;
     copy_src = (void *)(((char *)copy_src) + write_rc);     
   } while((bytesToRead > 0) && (write_rc >= 0));
   munmap(src_addr, pages*PAGE_SIZE);
}



/*
 * print sizes as "xxx kB", "xxx.y kB", "xxx MB" or "xxx.y MB" as needed;
 * allow for optional trailing string (like "\n")
 */
void print_size (ulong size, const char *s)
{
	ulong m, n;
	ulong d = 1 << 20;		/* 1 MB */
	char  c = 'M';

	if (size < d) {			/* print in kB */
		c = 'k';
		d = 1 << 10;
	}

	n = size / d;

	m = (10 * (size - (n * d)) + (d / 2) ) / d;

	if (m >= 10) {
		m -= 10;
		n += 1;
	}

	printf ("%2ld", n);
	if (m) {
		printf (".%ld", m);
	}
	printf (" %cB%s", c, s);
}

static void
print_type (image_header_t *hdr)
{
	char *os, *arch, *type, *comp;

	switch (hdr->ih_os) {
	case IH_OS_INVALID:	os = "Invalid OS";		break;
	case IH_OS_NETBSD:	os = "NetBSD";			break;
	case IH_OS_LINUX:	os = "Linux";			break;
	case IH_OS_VXWORKS:	os = "VxWorks";			break;
	case IH_OS_QNX:		os = "QNX";			break;
	case IH_OS_U_BOOT:	os = "U-Boot";			break;
	case IH_OS_RTEMS:	os = "RTEMS";			break;
#ifdef CONFIG_ARTOS
	case IH_OS_ARTOS:	os = "ARTOS";			break;
#endif
#ifdef CONFIG_LYNXKDI
	case IH_OS_LYNXOS:	os = "LynxOS";			break;
#endif
	default:		os = "Unknown OS";		break;
	}

	switch (hdr->ih_arch) {
	case IH_CPU_INVALID:	arch = "Invalid CPU";		break;
	case IH_CPU_ALPHA:	arch = "Alpha";			break;
	case IH_CPU_ARM:	arch = "ARM";			break;
	case IH_CPU_I386:	arch = "Intel x86";		break;
	case IH_CPU_IA64:	arch = "IA64";			break;
	case IH_CPU_MIPS:	arch = "MIPS";			break;
	case IH_CPU_MIPS64:	arch = "MIPS 64 Bit";		break;
	case IH_CPU_PPC:	arch = "PowerPC";		break;
	case IH_CPU_S390:	arch = "IBM S390";		break;
	case IH_CPU_SH:		arch = "SuperH";		break;
	case IH_CPU_SPARC:	arch = "SPARC";			break;
	case IH_CPU_SPARC64:	arch = "SPARC 64 Bit";		break;
	case IH_CPU_M68K:	arch = "M68K"; 			break;
	case IH_CPU_MICROBLAZE:	arch = "Microblaze"; 		break;
	default:		arch = "Unknown Architecture";	break;
	}

	switch (hdr->ih_type) {
	case IH_TYPE_INVALID:	type = "Invalid Image";		break;
	case IH_TYPE_STANDALONE:type = "Standalone Program";	break;
	case IH_TYPE_KERNEL:	type = "Kernel Image";		break;
	case IH_TYPE_RAMDISK:	type = "RAMDisk Image";		break;
	case IH_TYPE_MULTI:	type = "Multi-File Image";	break;
	case IH_TYPE_FIRMWARE:	type = "Firmware";		break;
	case IH_TYPE_SCRIPT:	type = "Script";		break;
	default:		type = "Unknown Image";		break;
	}

	switch (hdr->ih_comp) {
	case IH_COMP_NONE:	comp = "uncompressed";		break;
	case IH_COMP_GZIP:	comp = "gzip compressed";	break;
	case IH_COMP_BZIP2:	comp = "bzip2 compressed";	break;
	default:		comp = "unknown compression";	break;
	}

	printf ("%s %s %s (%s)", arch, os, type, comp);
}

void
print_image_hdr (image_header_t *hdr)
{
#if 0
	time_t timestamp = (time_t)ntohl(hdr->ih_time);
	struct rtc_time tm;
#endif

	printf ("\n   Image Name:   %.*s\n", IH_NMLEN, hdr->ih_name);
#if 0
	to_tm (timestamp, &tm);
	printf ("   Created:      %4d-%02d-%02d  %2d:%02d:%02d UTC\n",
		tm.tm_year, tm.tm_mon, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
#endif
	printf ("   Image Type:   "); print_type(hdr);
	printf ("\n   Data Size:    %d Bytes = ", ntohl(hdr->ih_size));
	print_size (ntohl(hdr->ih_size), "\n");
	printf ("   Load Address: %08x\n"
		"   Entry Point:  %08x\n",
		 ntohl(hdr->ih_load), ntohl(hdr->ih_ep));

	if (hdr->ih_type == IH_TYPE_MULTI) {
		int i;
		printf ("   Contents:\n");
    for (i = 0; i < imageCount; i++) {
			printf ("     Image %d: %8ld Bytes = ", i, imageSizes[i]);
			print_size (imageSizes[i], "\n");
		}
	}
	printf ("\n");
}


/* --------------------------------------------------------------------- */

void usage(void) {
  printf("\nusage: extimage [<options>]*\n\n"
	 "Where <options> can be:\n\n"
	 "  -h                 display this text\n"
	 "  -i <name>          input file name\n"
	 "  -o <name>          output file name ('-' or omit for stdout)\n"
	 "  -n <number>        image to extract\n"
	 "  -d                 display contains of input file\n\n");
  exit(0);
};

int main(int argc, char* argv[]) {
  
  int arg=1;
  char* s;
  char* cmd;
  int inFd = 0;
  int outFd = 0;
  int reqImageNum;

  if(argc==1) {
    usage();
    exit(0);
  };


  while(arg<argc) {
    
    if(strcmp("-h",argv[arg])==0) {
      usage();
    };
       
    if(strcmp("-i",argv[arg])==0) {
      arg++;
      inFd = open_input(argv[arg]);
    };
       
    if(strcmp("-o",argv[arg])==0) {
      arg++;
      outFd = open_output(argv[arg]);
    };

    if(strcmp("-n",argv[arg])==0) {
      arg++;
      reqImageNum = atoi(argv[arg]);
      extract = 1;
    };

    if(strcmp("-d",argv[arg])==0) {
      display = 1;
    };
    arg++;
  };

  if (!outFd) {
    outFd = 1; /* stdout */
  }
  if(extract) {
    if (inFd && outFd) {
       if (reqImageNum <= imageCount) {
         extract_image(reqImageNum, inFd, outFd);
       } else {
         printf("error: image file only contains %d images\n", imageCount);
       }
     } else {
       printf("error: extimage requires input and output files\n");
     }
  }

  if(display) {
    if (inFd) {
      print_image_hdr (&imageHdr);
    } else {
       printf("error: extimage no input file specified\n");
    }
  }
  return 0;
};
