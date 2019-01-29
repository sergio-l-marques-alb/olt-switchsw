##!/usr/bin/perl
########################################################################
###
###                LVL7 Confidential
###
### COPYRIGHT: (C) LVL7 Systems Inc. 2000. Property of LVL7.
###
### Module: l7tools\build\fix-custom.pl
###
### Function:
###      fix-custom.pl - A utility to fix an attempt to make an environment 
###      for a customer.  This file should be run if a user had already
###      custom.pl, but the build did not finish correctly, or some other
###      fix needs to be applied to either the open or closed source part
###      of the code.  This script will perform just like custom.pl, except
###      that it will not delete the newly created \src\product directory
###      and it will not attempt to remove the makefiles from the open-
###      source directories.  If a build fails due to syntax or other errors
###      this script will attempt to re-build another customer environment
###      from the broken one.
###
########################################################################
#
$openSrcFile = 'opensrc.cfg';
$FP_SRC = $ENV{FP_SRC};
$FP_HWSRC = $ENV{FP_HWSRC};
$bootos = "$FP_HWSRC\\ipl\\bootos";
$newDir = "$FP_SRC\\product";
$newDirS = "$newDir\\src";
$newDirH = "$newDir\\vit_hw\\src";
@openSrc;
@openSrcMake;
$objectFiles;
$dr;

sub init 
{
  # perform build but not link
  # just run build.bat but without the link
  #print "Checking for $bootos\n";
  #open(BOOTOS,$bootos) || die "bootos not created.  Check build.out for details.\n";
  #print "Bootos created successfully!\n";

  # clean all directories first
  print "Cleaning all object files....\n";
  system "perl codemake.pl all clean";
  #system "rm -rf $newDir";
  system "chmod 755 make.cfg";
  system "copy make.cfg.cust make.cfg";
}

sub config {

  open(OPENSRC,$openSrcFile) || die "Cannot find $openSrcFIle\nExiting.\n";
  while (<OPENSRC>) 
  {
    chomp;
    $_ =~ s/FP_SRC/$FP_SRC/g;
    $_ =~ s/FP_HWSRC/$FP_HWSRC/g;
    push @openSrc, $_; 
  }
  close(OPENSRC);
  # now @openSrc contains the list of directories that we
  # want to share.

  # create the new directory to put final product

  mkdir($newDir);
  mkdir($newDirS);
  mkdir($newDirH);

  # move all the src we want to save into the proper location
  foreach $dr (@openSrc) 
  {
    $slash = rindex($dr,"/"); 		## find the last slash
    $directory = substr($dr,$slash+1); ## get filename
    $path = substr($dr,0,$slash);	## get path
    if ($dr !~ /vit_hw/) 
    {
      system "xcopy /E /I /Q $path\\$directory\\*.* $newDirS\\$directory";
    }
    else 
    {
      system "xcopy /E /I /Q $directory\\*.* $newDirH";
    }
  }
}

sub cleanOpenSrc
{
  print "Entering 'makePrivate'";
  # build list of makefiles
  foreach $dr (@openSrc)
  {
    open(PATHS,"find $dr -name *.mak |") || die "Cannot exec 'find'\n";
    while (<PATHS>) 
    {
      chomp;
      #push @openSrcMake, $_;
      $_ =~ s/\//\\/g;
      $cmd = "rm -f $_";
      print "$cmd\n";
      system $cmd;
      #print "$_\n";
    }
    close(PATHS);
   }
}

sub makePrivate 
{

  system "pwd";
  system "perl codemake.pl all bldlib";

  print "Leaving 'makePrivate'\n";
}

sub archivePrivate 
{

  # build list of object files
  $path = "$FP_SRC\\l7tools\\build\\";
  print "$path\n";
  chdir($path);
  system "pwd";
  system "del private.src";
  system "del private.a";
  system "touch private.src";
  system "find $FP_SRC -name *.o >> private.src";
  system "armips -q private.a \@private.src";
  system "copy private.a $newDirH\\ipl";
  
  # now that we have made an archive of the private src, move
  # to that new directory and do a test build.
}

sub buildNew 
{  
  $path = "$newDirH\\ipl";
  chdir($path);
  system "pwd";
  #system "rm -f bootos.mak";
  system "copy bootos.mak.cust bootos.mak";
  #system "rm -f bootos-cust.mak";
  $path="$newDirS\\l7tools\\build";
  chdir($path);
  system "copy make.cfg.orig make.cfg";
  system "pwd";
  $ENV{FP_SRC} = $newDirS;
  $ENV{FP_HWSRC} = $newDirH;
  system "set";
  print "Cleaning....\n"; 
  $ret = `perl codemake.pl all clean`;
  print "Building....\n";
  $ret = `perl codemake.pl all bldlib`;
  print "Linking....\n";
  $ret = `perl codemake.pl all installexe`;


}
# call the functions we want to run

init();
#config();
#cleanOpenSrc();
makePrivate();
archivePrivate();
buildNew();
