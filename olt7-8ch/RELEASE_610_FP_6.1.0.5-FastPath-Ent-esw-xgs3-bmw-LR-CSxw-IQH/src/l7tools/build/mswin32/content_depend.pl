#! /usr/bin/perl
#######################################################################
##
##                LVL7 Confidential
##
## COPYRIGHT: (C) LVL7 Systems Inc. 2001-2002.
##
## Module: l7tools\build\content_depend.pl
##
## Function:
##      gomake.pl - A utility to run makefiles.
##
##
##
#######################################################################
use File::Find;
use File::Basename;

# set self to prog name
$self = basename($0);

sub usage {

print <<EOF;
\nUsage:\t$self [location]
\tlocation - sub-directory of FP_SRC where content.d is made


$self makes a content.d make depend file for all files and below

EOF

exit 1;

}

unless ($#ARGV >= 0) {&usage}

if ( $ARGV[0] =~ /help/i ){
   &usage($self);
}


#check if the FP_SRC environment variable has been set and is a directory
if (($ENV{"FP_SRC"} eq "") || (! -d $ENV{FP_SRC})) {
   print "\nERROR: $self: FP_SRC environment variable not set or not set properly.\n";
   &usage;

}
$fpsrc = $ENV{"FP_SRC"};
$fpsrc =~ tr/\\/\//;

#check if the FP_HWSRC environment variable has been set and is a directory
if (($ENV{"FP_HWSRC"} eq "") || (! -d $ENV{FP_HWSRC})) {
   print "\nERROR: $self: FP_HWSRC environment variable not set or not set properly.\n";
   &usage;

}

$fphwsrc = $ENV{"FP_HWSRC"};
$fphwsrc =~ tr/\\/\//;

# make sure the directory specified exists
if (!-e "$fpsrc/$ARGV[0]") {
   print "\nERROR:Invalid directory \"$ARGV[0]\" specified\n";
   &usage;
}
## get the root directory for the make to work on
$root = $fpsrc. "/".$ARGV[0];

# directory file list
find sub {push @makelist, $File::Find::name if /\.*$/ && !-d},$root;

open (OUTFILE,">$fpsrc/$ARGV[0]/contents");
#print OUTFILE "$fpsrc/$ARGV[0]/content.d: \\\n";
print OUTFILE "\$(OD)/ew_proto.h: \\\n";

foreach $file (@makelist)  		## process each mak file
{
   if (-1 != index($file,"$fpsrc/$ARGV[0]/contents")) {
   }
   else {
      print OUTFILE "$file \\\n";
   }
}

close OUTFILE;
chdir($root) || die "can't change to $root directory\n";

exit 0;
