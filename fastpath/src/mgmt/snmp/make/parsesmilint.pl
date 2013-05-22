use strict;

my $level = shift(@ARGV) || '2';
my $dmode = shift(@ARGV) || '';
my @file_list = @ARGV;

my %modules = ();
my %files = ();
my %module_imports = ();
my %ignore_file;

my $smilint_cmd = "smilint -s -l $level";

print "Loading mib module names from files...\n" if ($dmode eq 'verbose');
# map filenames to module names
foreach my $file (@file_list) {
   if ($file =~ /^\!(.*?)$/) {
      $ignore_file{$1} = 1;
      next;
   }
   if (open (MIB, "<$file"))
   {
      foreach my $line (<MIB>) {
         if ($line =~ /([A-Za-z0-9\-]+)\s+DEFINITIONS/) {
            $modules{$1} = $file;
            $files{$file} = $1;
            last;
         }
      }
      close(MIB);
   }
}

print "Determining individual imports...\n" if ($dmode eq 'verbose');
foreach my $module (sort(keys(%modules))) {
   &find_import($module);
}
#   if ($dmode eq 'verbose') {
#      foreach my $module (sort(keys(%module_imports))) {
#         print "$module -> ", join(',', @{$module_imports{$module}}), "\n";
#      }
#   }

my $error_count;
my @error_log;
my %ignore_list = map {$_ => 1} ('bridge.my','rfc1612_dnsresolver.my');

print "Ignoring files: ", join(' ', sort(keys(%ignore_file))), "\n" if ($dmode eq 'verbose');

print "Analyzing files...\n" if ($dmode eq 'verbose');
foreach my $file (@file_list) {
   next if ($file =~ /^\!/);
   next if ($ignore_file{$file});
   if (defined($files{$file})) {
      my @preload = &find_import_list($files{$file}, {});
      @preload = map {$modules{$_} || ()} @preload;
      my $cmd = "$smilint_cmd ".join(' ', map {"-p $_"} @preload)." $file";
      my @output = `$cmd 2>&1`;
      push @error_log, @output;
   }
}
#ignore errors for files that are ignored
@error_log = grep {$_ =~ /^(.*?)\:/ && !defined($ignore_file{$1})} @error_log;

print "Mapping errors to .mx file...\n" if ($dmode eq 'verbose');
my %map_files = ();
foreach my $error (@error_log) {
   if ($error =~ /^(.*?)\:(\d*)(.*?)$/) {
      my $filename = $1;
      my $linenum = $2;
      my $error_msg = $3;
      unless (defined($map_files{$filename})) {
         &parse_map_file($filename, \%map_files);
      }
      if (defined($map_files{$filename})) {
         my $new_filenum = $map_files{$filename}->{$linenum};
         $error = "${new_filenum}${error_msg}\n";
      }
   }
}

if (scalar(@error_log)) {
   print sort smilint_sort @error_log;
   print scalar(@error_log), " errors\n";
   exit -1;
}

exit 0;

sub find_import($) {
   my ($module) = @_;

   return if (defined($module_imports{$module}));

   my $file = $modules{$module};

   if (open(MIB, "<$file")) {
      my $imports = 0;
      my @imports = ();
      foreach my $line (<MIB>) {
         if ($line =~ /IMPORTS/) {
            $imports = 1;
            next;
         }
         if ($imports && $line =~ /FROM\s+([A-Za-z0-9\-]+)/) {
            my $module = $1;
            if ($module =~ /^(.*?)\;/) {
               push @imports, $1;
               last;
            }
            push @imports, $module;
         }
      }
      @imports = sort(@imports);

      $module_imports{$module} = \@imports;

      foreach my $module (@imports) {
         &find_import($module);
      }
      close(MIB);
   }
}

sub find_import_list($$)
{
   my ($base, $used) = @_;

   my @imports;
   if (defined($module_imports{$base})) {
      foreach my $module (@{$module_imports{$base}}) {
         if (! defined($used->{$module})) {
            $used->{$module} = 1;
            push @imports, &find_import_list($module, $used), $module;
         }
      }
   }
   return @imports;
}

sub smilint_sort() {
   my ($a_file, $a_line, $b_file, $b_line);
   if ($a =~ /^(.*?)\:(.*?)\:/) {
     $a_file = $1;
     $a_line = $2;

     if ($b =~ /^(.*?)\:(.*?)\:/) {
       $b_file = $1;
       $b_line = $2;

       if ($a_file eq $b_file) {
          return $a_line <=> $b_line;
       }
       else {
          return $a_file cmp $b_file;
       }
     }
   }
   return $a cmp $b;
}

sub parse_map_file($$) {
   my ($filename, $map_files) = @_;

   if (open(FILE, "<$filename.map")) {
      my %map_file;
      foreach my $line (<FILE>) {
         chomp($line);
         if ($line =~ /^(.*?),(.*?)$/) {
            $map_file{$1} = $2;
         }
      }
      $map_files->{$filename} = \%map_file;
   }
}
