use strict;
use File::Basename;

my @filenames = @ARGV;

unless (@filenames)
{
   print "Usage: $0 <file list/directory>\n";
   exit -1;
}

# if it's a file, just process the single file and return
if (-f @filenames[0])
{
   # if only looking at one file, print out all data
   if (@filenames == 1)
   {
      my %data = process_map(@filenames[0]);

      foreach my $field (sort(keys(%data)))
      {
         print "$field=$data{$field}\n";
      }
      exit;
   }
   unshift @filenames, '=';
   push @filenames, '=';
}
else
{
   # otherwise, if it's a directory, look in /ipl and /libs
   if (-d @filenames[0]) {
      my $path = @filenames[0];
      @filenames = ("=");

      # look for ipl map files
      push @filenames, "$path/ipl/bootos.map";
      push @filenames, "$path/ipl/switchdrvr.map";

      if (opendir(LIBS_DIR, "$path/libs")) {
         push @filenames, "-";
         foreach my $file (sort(readdir(LIBS_DIR))) {
            if ($file =~ /^fp_.*?\.map$/)
            {
               push @filenames, "$path/libs/$file";
            }
         }
         closedir(LIBS_DIR);
      }
      push @filenames, "=";
   }
   else
   {
      exit;
   }
}


#.bss, .comment, .ctors, .data, .debug_abbrev, .debug_aranges, .debug_info, .debug_line, 
#.dtors, .eh_frame, .rodata, .sbss, .sdata, .sdata2, .stab, .stabstr, .text, 
my @columns = ('.text', '.rodata', '.data', '.bss', '.stab', '.stabstr');
my %tabledata = ();
my @rownames = ();
my %columnwidths = ();

foreach my $file (@filenames)
{
   if ($file eq '-' || $file eq '=')
   {
      # if it's a line, just put it on the rownames list
      push @rownames, $file;
   }
   elsif (-f $file)
   {
      &process_file($file);
   }
   #ignore if not found
}

my $pagewidth = $columnwidths{'Component'};
#print out column headers
print (' ' x $columnwidths{'Component'});
foreach my $column (@columns) {
   # don't print it if there's no data
   if (defined($columnwidths{$column})) {
      $columnwidths{$column} = length($column) if ($columnwidths{$column} < length($column));
      print sprintf("  %$columnwidths{$column}s", $column);
      $pagewidth += 2+$columnwidths{$column};
   }
}
print "\n";

#print out data;
foreach my $rowname (@rownames) {
   if ($rowname eq '-' || $rowname eq '=') {
      print ($rowname x $pagewidth);
      print "\n";
      next;
   }
   print sprintf("%-$columnwidths{'Component'}s", $rowname);
   foreach my $column (@columns) {
      print sprintf("  %$columnwidths{$column}s", $tabledata{"$rowname.$column"});
   }
   print "\n";
}

#.bss, .comment, .ctors, .data, .debug_abbrev, .debug_aranges, .debug_info, .debug_line, 
#.dtors, .eh_frame, .rodata, .sbss, .sdata, .sdata2, .stab, .stabstr, .text, 

# process a file
sub process_file($filepath)
{
   my ($filepath) = @_;

   return unless (open(FILE, "<$filepath"));

   # store every valid line in hash
   my %rowdata = ();

   # iterate over each line in the file
   foreach my $line (<FILE>)
   {
      $rowdata{$1} = hex($2) if ($line =~ /^(\.\w*?)\s+0x\w*?\s+0x(\w*?)$/);
   }
   close(FILE);

   my $rowname = basename($filepath);
   $rowname = $1 if ($rowname =~ /^(.*?)\.map$/);  #strip off .map
   $rowname = $1 if ($rowname =~ /^fp_(.*?)$/);    #strip off fp_

   push @rownames, $rowname;
   if (!defined($columnwidths{'Component'}) || ($columnwidths{'Component'} < length($rowname)))
   {
      $columnwidths{'Component'} = length($rowname);
   }

   foreach my $column (@columns)
   {
      if (defined($rowdata{$column}))
      {
         my $val = &comma($rowdata{$column});
#         print "$rowname.$column => $val ($rowdata{$column})\n";
         $tabledata{"$rowname.$column"} = $val;
         if (!defined($columnwidths{$column}) || ($columnwidths{$column} < length($val)))
         {
            $columnwidths{$column} = length($val);
         }
      }
      else
      {
         $tabledata{"$rowname.$column"} = 0;
      }
   }
}

sub comma($val)
{
   my ($val) = @_;
   return $val if ($val == 0 && $val ne '0');

   # get the digits in reverse order
   my @digits = split(/ */, $val);
   my @output = ();

   while (@digits)
   {
      # pull off atleast 3 characters from end and put them on the front of the list
      my $group == '';
      for (my $count = 3; $count >0; $count--) {
         my $digit = pop(@digits);
         $digit = '' if (!defined($digit));
         $group = "$digit$group";
      }

      unshift @output, $group;
   }
   # make comma-delimited
   join(',',@output);
}


sub process_map()
{
   my ($filepath) = @_;

   my %data = ();

   return %data unless (open(FILE, "<$filepath"));
   
   # iterate over each line in the file
   foreach my $line (<FILE>)
   {
      $data{$1} = hex($2) if ($line =~ /^(\.\w*?)\s+0x\w*?\s+0x(\w*?)$/);
   }

   close(FILE);
   return %data;
}
