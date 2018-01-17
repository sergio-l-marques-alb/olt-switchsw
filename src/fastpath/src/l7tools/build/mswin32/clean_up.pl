#! /usr/bin/perl
#######################################################################
##
##                LVL7 Confidential
##
## COPYRIGHT: (C) LVL7 Systems Inc. 2000. Property of LVL7.
##
## Module: build\clean_up.pl
##
## Function:
##      clean_up.pl - A utility to remove files after a build.
##
##
##
#######################################################################
## $Log: lvl7dev\src\l7tools\build\clean_up.pl  $
## Revision 1.2 2001/07/25 00:06:06EDT rjindal 
## Revision 1.1 2001/06/14 19:21:51EDT mfiorito 
## Initial revision
## Revision 1.2 2001/04/02 13:06:31EDT bmutz 
## Revision 1.1 2001/03/31 09:24:17EST bmutz 
## Initial revision
##
#######################################################################

#check if the FP_SRC environment variable has been set and is a directory
if (($ENV{"FP_SRC"} eq "") || (! -d $ENV{FP_SRC})) {

   print "\nERROR: clean_up: \FP_SRC environment variable not set or not\n";
   print "                    set properly.\n\n";
}

#check if the FP_HWSRC environment variable has been set and is a directory
if (($ENV{"FP_HWSRC"} eq "") || (! -d $ENV{FP_HWSRC})) {

   print "\nERROR: clean_up: \FP_HWSRC environment variable not set or not\n";
   print "                    set properly.\n\n";
}

## ipl directory is the FP_HWSRC environment variable
$ipl = sprintf("%s/%s/",$ENV{"FP_HWSRC"},"ipl");
print "$ipl\n";

chdir($ipl) || die "Can't change to $ipl directory\n";
system("rm tmp.* >/dev/null 2>&1");
system("rm libsw.a >/dev/null 2>&1");

exit 0;

