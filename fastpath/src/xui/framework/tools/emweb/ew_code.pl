#!/usr/bin/perl -W
#
# Author: Dana Fowler
#
# Usage: perl ew_code.pl
#
# This function parses the emweb_string function in ew_code.c
# into separate functions for each generated block of code, this
# requires far less stack usage (~130K, depending on package combination),
# than an implementation using one function.
#
# CRM: use a jump table, to keep the great big switch statement from 
# overloading the MIPS compiler. 
$arg = shift(@ARGV);

my $out = $arg;
my $in  = $arg . ".tmp";

my @emweb_string = ();  #store emweb_string function
my $loc = 1;            # store lines of code to fix #line directives
# regex to match case statements and line directives
my $match_case = '^\s*case\s+([0-9]+):\s*$';
my $match_line = '^#line\s+[0-9]+\s+"ew_code\.c"\s*$';

# move ew_code.c to temp file for reading/parsing, write out ew_code.c
system "mv $out $in";  

open(IN, "$in") || die "Can't open $in!\n";
open(OUT, ">$out") || die "Can't open $out!\n";

# just copy lines to ew_code.c until we get to emweb_string, will hit comments first...
while (defined($line = <IN>))
{
  if ($line =~ /emweb_string/) { last; }
  print OUT $line;
  $loc++;
}

push(@emweb_string, $line); # save comment line

# write comments for added case functions.
print OUT "* The following functions invoke code generated for each emweb_string\n";
print OUT "* statement found in the archive source files.\n";
print OUT "* They are parsed out of the ewc generated emweb_string function by ew_code.pl\n";
print OUT "*/\n";  # end comment separator
print OUT "\n";
$loc += 5;

my $case = 0;  # case block we are working on.
my $end = 0;   # set to 1 when we hit end of generated code segment.
my $max_case_num = 0;
# just copy lines of code in emweb_string until we hit a case statement > 0...
while (defined($line = <IN>))
{
  if (($line =~ /$match_case/) && (eval($1) > 0)) 
  { 
    $case = $1;
    last; 
  }
  push(@emweb_string, $line);
}

print OUT "static const void *emweb_string_case$case(EwsContext ewsContext)\n";
$loc++;

while (defined($line = <IN>))
{
  if ($end == 0) 
  {
    # check for end of generated code block
    if ($line =~ /\/\* End of generated code \*\//) 
    { 
      $end = 1;
      print OUT "return NULL; /* default return statement to catch any snippets w/o return */";
      $loc++;
    }
  } else
  {
    # end loop on final default case block
    if ($line =~ /default:/) { last; }  

    # skip all the extra break statements after generated code ...
    if ($line =~ /break;/) { next; }

    # match on any case number and save case number ...
    if ($line =~ /$match_case/)  
    {
      $case = $1;
      print OUT "static const void *emweb_string_case$case(EwsContext ewsContext)\n";
      $loc++;
      $end = 0;  # at beginning of generated code
      next;
    }
  }
  # fix up #line directives for ew_code.c with current loc
  if ($line =~ /$match_line/) 
  {
    print OUT "#line $loc \"ew_code.c\"\n";
  } else
  {
    print OUT $line;  # print generated code to function.
  }
  $loc++;
}

print OUT 'static const void *(*jump_table[])(EwsContext) = {NULL,' . "\n";
$loc++;
my $i;
for ($i=1;$i<$case;$i++) {
    print OUT '    emweb_string_case' . $i . ",\n";
    $loc++;
}
print OUT '    emweb_string_case' . $case . '};' . "\n";
$loc++;

print OUT "\n";
print OUT "/**********************************************************************\n";
$loc += 2;

# print all emweb_string function lines we saved in @emweb_string...
foreach $value (@emweb_string) 
{
  print OUT $value;
  $loc++;
}

print OUT $line;  # print last line we read, default: .
$loc++;

# Insert call to our jump table here. 
# 'case' variable is the maximum case number. 
print OUT '            if ((indx >=1) && (indx <= ' . $case . ')) {' . "\n";
$loc++;
print OUT '                return ((jump_table[indx])(ewsContext));' . "\n";
$loc++;
print OUT '            }' . "\n";
$loc++;
# read and write remaining lines to ew_code.c
while (defined($line = <IN>))
{
  # fix up #line directives for ew_code.c with current loc
  if ($line =~ /$match_line/) 
  {
    print OUT "#line $loc \"ew_code.c\"\n";
  } else
  {
    print OUT $line;
  }
  $loc++;
}

close IN;
close OUT;

# remove temporary file
unlink($in);

exit 0;
