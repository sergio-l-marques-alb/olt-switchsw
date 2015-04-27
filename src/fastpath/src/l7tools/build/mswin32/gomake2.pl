#! /usr/bin/perl
#######################################################################
##
## Module: build\gomake.pl
##
## Function:
##      gomake.pl - A utility to run Roadrunner makefiles.
##
## Change Summary:
##   date   SID Init    Action
## ----------------------------
## 08/05/99 1.0 gaunce  Created based on gomake.pl but all output,
##                      stderr and stdout, is redirected to the log
##                      file.  Useful for Win9x users.
##
#######################################################################

sub printusage {

   print  "\nUsage:\tgomake [component [make_options] ]\n";
   print  "\t\tcomponent - name of component to be made\n";
   print  "\t\tmake_options - options to be passed to make command\n";
   print  "\n";
   print  "gomake runs makefiles for the component specified, or\n";
   print  "all makefiles below the current directory if what_to_make\n";
   print  "is specified as \"all\" or if no optional arguments are\n";
   print  "specified to it.\n";
   print  "\n";
   print  "gomake was written to perform builds of Roadrunner components,\n";
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


#procs to list subdirs in a tree
#result is stored in a global list @dirrez
#invoked with: &list_dirs(startingdir);


sub read_subdirs {
   local($testdir);
   local(@filenames);
   local(@dirlist);
   $tstdir = "." ;
   opendir (DIR,$tstdir) || die "Can't open directory";
   @filenames = grep (!/^\.\.?$/,readdir(DIR));
   close(DIR);
   foreach (@filenames){
      if (-d) {
         push(@dirlist,$_);
      }
   }
   @dirlist;
}

sub dscnd {
   local($from,$to) = @_;
   local($param);
        local($old_dir);
        local($rc);
   local(@my_list);
        push(@dirrez,sprintf ("%s/%s",$from,$to));
   $old_dir = $from;
   $rc = chdir($to);
        if ($rc == 0) {
                printf (stderr "\nCouldn't go to %s from %s Erro = %d\n",$to,$from,$!);
                #return;
                exit 0;
        }
   @my_list = &read_subdirs();
   $param = $from."/".$to;
   while (@my_list) {
           &dscnd($param,$my_list[0]);
           shift @my_list;
        }
        chdir($old_dir);
}

sub list_dirs {
   local ($arg) = @_;
   local ($rc);
   local (@list);
   local ($here);
   print "\Starting at $arg to compile sdir list\n";
   $rc = chdir ($arg);
   if ($rc == 0) {
     printf (stderr "\nCouldn't start from %s Erro = %d\n",$to,$!);
     #return;
     exit 0;
   }
   @list = &read_subdirs();
   @dirrez = $arg;
   while (@list) {
      $here = $arg;
      &dscnd($here,$list[0]);
           shift @list;
   }
}





sub exec_make {

  my $dirname = $_[0];
  print "\nVisiting $dirname\n";
  chdir ($dirname);


  opendir(DIR,'.');
  @mak = grep(/\.mak$/, readdir(DIR));
  closedir(DIR);
  # now iterate through all of the *.mak (make) files
  # also, show the output on the screen to let the
  # user know what's going on
  foreach (@mak) {

     @tokens = split /.mak/;
     $filename = $tokens[0];
     print "\nExecuting makefile : $_\n\n";

     if ($ARGV[0] eq "clean") {
        $rc = system ("make -k -f $_ @ARGV");

     }

     else {
        open(SAVEOUT, ">&STDOUT");
        open(SAVEERR, ">&STDERR");
        open(STDOUT, ">>$filename.mak.log");
        open(STDERR, ">&STDOUT");
        select(STDERR); $| = 1; #make unbuffered
        select(STDOUT); $| = 1; #make unbuffered

        $rc = system ("make -f $_ @ARGV");

        close(STDOUT);
        close(STDERR);
        open(STDOUT, ">&SAVEOUT");
        open(STDERR, ">&SAVEERR");

     }

     # get out on the first error
     if ($rc) {

        print "\nMake command failed while executing make on makefile:\n\n";
        print "\t $dirname\\$_\n";

     }

  }

  chdir ($ENV{"FP_SRC"});


}

$rc = 0;

# check for no arguments; if there are none, then set the components
# to build equal to "All"
if ( $ARGV[0] eq "" ) {

   $comp = "All";

}
# check if the user wants help
elsif ( ($ARGV[0] =~ /\?/) || ($ARGV[0] =~ /help/i) ) {

   printusage;

}
# otherwise, build only what the user wants to build
else {

   $comp = $ARGV[0];
   shift (@ARGV);
   #print "\n\nThe rest of the command line arguments are: @ARGV.\n\n"

}

#check if the FP_SRC environment variable has been set and is a directory
if (($ENV{"FP_SRC"} eq "") || (! -d $ENV{FP_SRC})) {

   print "\nERROR: gomake: \FP_SRC environment variable not set or not\n";
   print "               set properly.\n\n";
   printusage;

}


print "\nCompiling a list of directories\n";
list_dirs($ENV{"FP_SRC"});

# if the user specified nothing, then do everything
if ( $comp =~ /all/i ) {

   # now find all of the sub-directories
   #open (FILE_NAMES, "dir $ENV{\"FP_SRC\"} /a:d /s|");
   #@file_names = <FILE_NAMES>;
   #close (FILE_NAMES);

   #if looking for all, don't worry about it

}

# the user must have specified a particular component to build
else {

   if (-d $ENV{"FP_SRC"} . "\\$comp") {

      # now find all of the sub-directories
      #open (FILE_NAMES, "dir $ENV{\"FP_SRC\"}\\$comp /a:d /s|");
      #@file_names = <FILE_NAMES>;
      #close (FILE_NAMES);
      $comp = $ENV{"FP_SRC"} . "\\$comp";

   }

   else {

      print "\nComponent $ENV{\"FP_SRC\"}\\$comp not found.\n";
      exit 2;

   }

}
$done = 0;
$todo = scalar(@dirrez);
if ($comp =~ /all/) {
   foreach (@dirrez) {
   #foreach (@file_names) {

     # get rid of the newline character
      #chop;

      # check for the word "Directory"
      #if (/Directory/) {

        #s/^ // ; #if NT kill the leading space
       #@tokens = split / /, $_;

        #$dirname = $tokens[2];
        # don't go into directories where you don't want to do a "make"
        #if (($dirname !~ /headers/) && ($dirname !~ /l7tools/build/) &&
       #($dirname ne $ENV{"FP_SRC"})) {
      printf("\n************ STATUS ****************\n");
      printf("      Directories: processed %d \n",$done);
      printf("                   remaining %d\n",($todo - $done));
      exec_make ($_);
      $done++;
   }
}
else {
   $comp =~ s/\\/\//gi;
   $done = 0;
   foreach (@dirrez) {
      s/\\/\//gi;
      if (/$comp/) {
         push(@tmplist,$_);
      }
   }
   $todo = scalar(@tmplist);
   foreach (@tmplist) {
      printf("\n************ STATUS ****************\n");
      printf("      Directories: processed %d \n",$done);
      printf("                   remaining %d\n",($todo - $done));
      exec_make ($_);
      $done++;
   }
}

exit $rc;
