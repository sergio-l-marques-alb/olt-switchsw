#!/usr/bin/perl
use strict;
use Getopt::Long;

my $metafile;
my $output;
my $mibfile;
my $customer;
my $debug=0;
my $help=0;
my $test=0;
my $nomap=0;

GetOptions(
    'metafile=s' => \$metafile,
    'mibfile=s'  => \$mibfile,
    'customer=s' => \$customer,
    'output=s'   => \$output,
    'debug'      => \$debug,
    'help'       => \$help,
    'test'       => \$test,
    'nomap'      => \$nomap,
    );

if ($help || ((!$metafile || !$mibfile) && !$test))
{
   print "Usage: processmib.pl --metafile <file.mx path>\n",
         "                     --mibfile <file.my path>\n",
         "                     [--customer <customer package path>]\n",
         "                     [--output <output directory>]\n",
         "                     [-debug] [-help] [-test] [-nomap]\n";
}

if ($test) {
   open(META, "<&DATA");
   open(MIB, ">&STDOUT");
}
else {
   unless (open(META, $metafile))
   {
      print "Could not open metafile '$metafile' for reading.";
      exit;
   }

   unless (open(MIB, ">$mibfile"))
   {
      print "Could not open output file '$mibfile' for writing.";
      exit;
   }

   unless ($nomap) {
      unless (open(MAP, ">$mibfile.map"))
      {
         print "Could not open output file '$mibfile.map' for writing.";
         exit;
      }
   }
}

my %values = ();

my $metafile_name = $metafile;
if ($metafile_name =~ /[\\\/]([^\\\/]*)$/) {
   $metafile_name = $1;
}

foreach my $file ("$output/flex.h", 
                  "$output/feature.h", 
                  "$customer/customer_defines") {
   if (-f $file) {
      unless (open(DEFINES, "$file"))
      {
         print "Could not open $file at '$output' for reading.";
         exit;
      }
   
      foreach my $line (<DEFINES>)
      {
         chomp($line);
         if ($line =~ /#define (L7_.*?_PACKAGE|L7_FEAT_.*?) (\d)/i)
         {
            $values{lc($1)} = $2;
            print "Add tag: ".lc($1)."=$2\n" if ($debug);
         }
      }
      close(DEFINES);
   }
}
if (-f "$output/package.cfg")
{
   unless (open(PACKAGE, "$output/package.cfg"))
   {
      print "Could not open $output/package.cfg for reading.";
      exit;
   }

   foreach my $line (<PACKAGE>)
   {
      chomp($line);
      if ($line =~ /export (.*?)=(.*?)/i)
      {
         $values{lc($1)} = lc($2);
      }
   }
   close(PACKAGE);
}

#####
my @conditional;  # if defined($conditional[-1]), then in #if block
                  # if $conditional[-1], then allow, swap on #else
my $include=0;
my $include_val=0; #if 1, and $include=1,then allow, swap on #endinclude
my $line_no = 0;
my $my_line_no = 0;

$line_no = 268 if ($test);

foreach my $line (<META>)
{
  $line_no++;

  #perform value substitution
  while ($line =~ /^(.*?)\$\(([^\(\)]+?)\)(.*?)$/) {
     my $sub = $values{$2} || '';
     $line = "$1$sub$3";
  }

  # check to see if we're in a block and should skip this line
  if ($line =~ /\s*--\s*#IF\s+?([a-zA-Z0-9,\-_!]*?)\s*$/i)
  {
     my $val = &checkconditional('if',$1);
     push @conditional, $val;
     next;
  }
  if ($line =~ /\s*--\s*#IFDEF\s+?([a-zA-Z0-9,\-_!]*?)\s*$/i)
  {
     my $val = &checkconditional('ifdef',$1);
     push @conditional, $val;
     next;
  }
  elsif ($line =~ /\s*--\s*#ELSE\s*/i)
  {
     unless (defined($conditional[-1])) {
        print STDERR "$metafile_name($line_no): #else with no matching #if\n";
        exit -1;
     }
     $conditional[-1] = !($conditional[-1]);
     next;
  }
  elsif ($line =~ /\s*--\s*#ENDIF\s*/i)
  {
     unless (defined($conditional[-1])) {
        print STDERR "$metafile_name($line_no): #endif with no matching #if\n";
        exit -1;
     }
     pop @conditional;
     next;
  }
  elsif ($line =~ /\s*--\s*#ENDINCLUDE\s*/i)
  {
     $include = 0;
     next;
  }
  elsif ($line =~ /\s*--\s*#INCLUDE\s+?([a-zA-Z0-9\-_]*?)\s*$/i)
  {
     my $include_section = $1;
     $include = 1;
     $include_val = 0;
     my $found = 0;

     # perform include
     if (open(FILE, "<$customer/mibs/customer_subs"))
     {
        my $sub_line_no = 0;
        foreach my $line (<FILE>) {
           $sub_line_no++;
           chomp($line);
           if ($found) {
              if ($line =~ /^--/) {
                 last;
              }
              else {
                 $my_line_no++;
                 print MIB "$line\n";
                 print MAP "${my_line_no},${customer}/mibs/customer_subs:${sub_line_no}\n" unless($nomap);
              }
           }
           else {
              if ($line =~ /^$include_section\s*$/) {
                 $found = 1;
              }
           }
        }
     }
     unless ($found) {
        $include_val = 1;
     }
     next;
  }
  elsif ($line =~ /\s*--\s*#DEFINE\s+?([a-zA-Z0-9\-_!]*?)\s*$/i)
  {
     $values{lc($1)} = 0;
     print "Define: ",lc($1),"\n" if ($debug);
     next;
  }
  elsif ($line =~ /\s*--\s*#DEFINE\s+?([a-zA-Z0-9\-_!]*?)\s+([0-9]*?)$/i)
  {
     $values{lc($1)} = $2;
     print "Define: ",lc($1)," = $2\n" if ($debug);
     next;
  }

  # verify all conditionals are true before printing
  my $print_line = 1;
  foreach my $cond (@conditional) {
     $print_line = 0 if (!($cond));
  }
  $print_line = 0 if ($include && !$include_val);
  if ($print_line)
  {
     $my_line_no++;
     print MIB $line;
     print MAP "${my_line_no},$metafile_name:${line_no}\n" unless($nomap);
  }
}
close(MIB);
close(MAP) unless ($nomap);

if (scalar(@conditional)) {
   print STDERR "$metafile_name($line_no): Unterminated #if or #ifdef\n";
   exit -1;
}

sub checkconditional($conditional, $keywords)
{
   my ($conditional, $keywords) = @_;

   print "($line_no) Check: '$keywords':\n" if ($debug);
   my @keywords = grep {length($_)} split(/\s*,\s*/,$keywords);

   foreach my $keyword (@keywords)
   {
      # determine operation (!)
      my $op = 1;
      if ($keyword =~ /^\!(.*)$/) {
         $op = 0;
         $keyword = $1;
      }

      # get value
      my $val = 0;
      if ($conditional eq 'ifdef') {
         $val = defined($values{lc($keyword)});
      }
      elsif ($conditional eq 'if') {
         $val = $values{lc($keyword)} || 0; 
      }

      # return 0 if failure
      $val = ($op ? $val : !($val));
      print "\t$conditional($keyword) = $val\n" if ($debug);
      return 0 unless ($val);
   }
   return 1;
}


# Test cases
__DATA__
--#define DEFINE
--#define TRUE  1
--#define FALSE 0
ifdef:
--#ifdef DEFINE
Test 1a: PASS
--#else
Test 1a: FAIL
--#endif
--#ifdef !DEFINE
Test 1b: FAIL
--#else
Test 1b: PASS
--#endif

if:
--#if TRUE
Test 2a: PASS
--#else
Test 2a: FAIL
--#endif
--#if !TRUE
Test 2b: FAIL
--#else
Test 2b: PASS
--#endif

else:
--#if FALSE
Test 3a: FAIL
--#else
Test 3a: PASS
--#endif
--#if !TEST3
Test 3b: PASS
--#else
Test 3b: FAIL
--#endif

nested if:
--#if TRUE
--#if TRUE
Test 4: PASS
--#else
Test 4: FAIL 1
--#endif
--#else
Test 4: FAIL 2
--#endif

nested else:
--#if FALSE
Test 5: FAIL 1
--#else
--#if TRUE
Test 5: PASS
--#else
Test 5: FAIL 1
--#endif
--#endif

nested else else:
--#if FALSE
Test 6a: FAIL 1
--#else
--#if FALSE
Test 6a: FAIL 2
--#else
Test 6a: PASS
--#endif
--#endif
--#if FALSE
--#if FALSE
Test 6b: FAIL 1
--#else
Test 6b: FAIL 2
--#endif
--#else
--#if FALSE
Test 6b: FAIL 3
--#else
Test 6b: PASS
--#endif
--#endif
