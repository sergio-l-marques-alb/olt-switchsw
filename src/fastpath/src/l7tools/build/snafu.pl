#Recurively find errors in *.mak.log files
#procs to list subdirs in a tree
#result is stored in a global list @dirrez
#invoked with: &list_dirs(startingdir);
#optionally pass a second parameter (of any value) to quiesce non-error output

use Cwd;

my $quiet=($ARGV[1]);

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
   local($rc);
   local(@my_list);
   push(@dirrez,sprintf ("%s/%s",$from,$to));
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
   chdir("..");
}
   
sub list_dirs {
   local ($arg) = @_;
   local ($rc);
   local (@list);
   local ($here);
   local ($start);
   $start = &Cwd::cwd();
   print "\Starting at $arg to compile sdir list\n" unless $quiet;
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
   chdir ($start);
}

print "Starting to look for problems\n" unless $quiet;
&list_dirs($ARGV[0]);
foreach (@dirrez) {
   local ($start);
   $start = &Cwd::cwd();
   chdir($_);
   $currentdir = $_;

   opendir(DIR,'.');
   @maklog = grep(/\.log$/, readdir(DIR));
   closedir(DIR);

   newlog:
   foreach (@maklog) {
      $currentfile = $_;
      open (INFILE,$_);
      while (<INFILE>) {
         if (/ Error /) {
            print "Error in $currentdir/$currentfile\n";
            close INFILE;
            next newlog;
         }
      }
      close INFILE;
   }
   chdir ($start);
}
printf "That's all Folks!\n" unless $quiet;

