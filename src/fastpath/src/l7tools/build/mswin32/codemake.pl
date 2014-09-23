#! /usr/bin/perl
#######################################################################
##
##                LVL7 Confidential
##
## COPYRIGHT: (C) LVL7 Systems Inc. 2000. Property of LVL7.
##
## Module: l7tools\build\gomake.pl
##
## Function:
##      gomake.pl - A utility to run Roadrunner makefiles.
##
##
##
#######################################################################
## $Log: lvl7dev\src\l7tools\build\codemake.pl  $
## Revision 1.1.1.4 2001/10/15 19:04:08EDT bmutz 
## Revision 1.4 2001/10/05 22:27:34EDT ssaraswatula 
## Revision 1.2 2001/07/25 00:06:30EDT rjindal 
## Revision 1.1 2001/06/14 19:21:51EDT mfiorito 
## Initial revision
## Revision 1.16 2001/03/26 01:13:45EST mbaucom 
## Don't need to force a compile of the MMC directory first anymore
## Revision 1.14 2000/10/28 17:05:30EDT bmutz
## Revision 1.13  2000/10/27 11:57:13  bmutz
## build ucode directory first
## Revision 1.12  2000/08/17 14:50:52  bmutz
## fix indpendent directory builds
## Revision 1.11  2000/07/20 12:26:26  bmutz
## support for src and hwsrc environrment variables
## Revision 1.10  2000/07/19 13:24:15  bmutz
## Add build support
## Revision 1.9  2000/07/05 11:28:44  sledford
## Add build support.
## Revision 1.8  2000/06/28 12:33:33  sledford
## Add build support.
## Revision 1.7  2000/03/13 13:03:16  mbaucom
## testing
## Revision 1.6  2000/03/13 18:03:16Z  mbaucom
## Working on the appearance of $log
## Revision 1.5  2000/03/13 17:04:36Z  mbaucom
## still working on Keywords
##
#######################################################################


sub printusage {
   local($prog) = @_;
   print  "\nUsage:\t$prog [component [make_options] ]\n";
   print  "\t\tcomponent - directory based from FP_SRC to be made\n";
   print  "\t\tmake_options - options to be passed to make command\n";
   print  "\n";
   print  "$prog runs makefiles for the component specified, or\n";
   print  "all makefiles below the current directory if what_to_make\n";
   print  "is specified as \"all\" \n";
   print  "\n";
   print  "$prog was written to perform builds of Roadrunner components,\n";
   print  "so it requires that environment variable FP_SRC be set when it is\n";
   print  "invoked.\n";
   print  "\n";
   print  "It makes a component by descending the directory structure\n";
   print  "until it finds that component's directory, then running\n\n";
   print  "\tmake [options]\n\n";
   print  "on all *.mak files found in that directory and directories\n";
   print  "below it.\n";
   print  "\n";
   exit 1;

}


unless ($#ARGV >= 0) {die "Usage: $0 <dir> <flag>\n       $0 help for help\n";}

if ( $ARGV[0] =~ /help/i ){
	printusage($0);
}


#check if the FP_SRC environment variable has been set and is a directory
if (($ENV{"FP_SRC"} eq "") || (! -d $ENV{FP_SRC})) {

   print "\nERROR: gomake: \FP_SRC environment variable not set or not\n";
   print "               set properly.\n\n";
   printusage;

}

#check if the FP_HWSRC environment variable has been set and is a directory
if (($ENV{"FP_HWSRC"} eq "") || (! -d $ENV{FP_HWSRC})) {

   print "\nERROR: gomake: \FP_HWSRC environment variable not set or not\n";
   print "               set properly.\n\n";
   printusage;

}

if ($ARGV[0] =~ /all/i )
{
	## root directory is the fp_src environment variable
	$root = sprintf("%s/",$ENV{"FP_SRC"});
}else
{
	## get the root directory for the make to work on
	$root = sprintf("%s/%s",$ENV{"FP_SRC"},$ARGV[0]);
}

$log = 0;

if (($#ARGV == 2) && ($ARGV[2] =~ /showlog/i)) {
   $log = 1;
}

## change to the root directory
if ($ARGV[0] =~ /all/i )
{
        print "$root\n";
        ## root directory is the FP_SRC environment variable
        $root1 = sprintf("%s",$ENV{"FP_SRC"});
        chdir($root1) || die "Can't change to $root1 directory\n";
        system("find $root1 -name '*.mak' -print | sort > $root/makelist.txt");
        $root2 = sprintf("%s",$ENV{"FP_HWSRC"});
        chdir($root2) || die "Can't change to $root2 directory\n";
        system("find $root2 -name '*.mak' -print  | sort>> $root/makelist.txt");
        chdir($root) || die "Can't change to $root directory\n";
}else
{
        ## get the root directory for the make to work on
        chdir($root) || die "Can't change to $root directory\n";
        system("find $root -name '*.mak' -print  | sort> $root/makelist.txt");

}

# Read the list and operate on it
open(IN,"makelist.txt");

## get the arguments for the make command
$bldlib = 0;
if($ARGV[1] =~ /clean/i) {
   $baseCmd = sprintf("make -k -f");
   $clean = 1;
}
else {
   if($ARGV[1] =~ /bldlib/i) {
      $bldlib = 1;
   }
   $baseCmd = sprintf("make -f");
   $clean = 0;
}


while (<IN>)
{
	chop; 										## get rid of the new line char
	push(@mylist,$_);
}

while(@mylist)  								## process each mak file
{
	print "\n---------------------------------------------------------------\n";
	print $#mylist+1 . " directories to process\n";
	$entry = shift(@mylist);
	print $entry . " Is the working directory\n";
	$slash = rindex($entry,"/"); 				## find the last slash
	$makefile = substr($entry,$slash+1); 		## get filename
	$directory= substr($entry,0,$slash);		## get path
	## build the make command
	chdir($directory);							## directory for the command
        if ($clean) {
           $theCommand = sprintf("%s %s %s \n",$baseCmd,$makefile,$ARGV[1]);
           $rc = system($theCommand);
        }else{
            open(SAVEOUT, ">&STDOUT");
            open(SAVEERR, ">&STDERR");
            if ($bldlib) {
               open(STDOUT, ">$makefile.log");
            }else{
               open(STDOUT, ">>$makefile.log");
            }
            open(STDERR, ">&STDOUT");
            select(STDERR); $| = 1; #make unbuffered
            select(STDOUT); $| = 1; #make unbuffered

            $theCommand = sprintf("%s %s %s \n",$baseCmd,$makefile,$ARGV[1]);
            $rc = system($theCommand);

            close(STDOUT);
            close(STDERR);
            open(STDOUT, ">&SAVEOUT");
            open(STDERR, ">&SAVEERR");

        }
##	if ($rc){
##	   print "make failed on " . $entry . "\n";
##           system(sprintf("type %s\n",$makefile . ".log"));
##           print "make failed on " . $entry . "\n";
##        }
        if($log){
           system(sprintf("cat %s\n",$makefile . ".log"));
        }
}

close(IN);

chdir($root) || die "can't change to $root directory\n";
system("rm makelist.txt"); 					## delete the make list

exit 0;



