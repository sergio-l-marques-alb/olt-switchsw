#!/usr/bin/perl -W
#
# Author: Matthew Smith, msmith@lvl7.com
#

$cfile = "symTbl.c";
$hfile = "symTbl.h";
$funcs = 0;
#$nmpath = $ENV{"NM"};
$nmpath = $ARGV[-1];

open(IN, "$nmpath switchdrvr | grep \" T \"|") || die "Can't open pipe for nm: $!\n";
open(H, ">$hfile") || die "Can't open $hfile: $!\n";
open(C, ">$cfile") || die "Can't open $cfile: $!\n";

print C "#include \"symTbl.h\"\n\ntypedef struct\n{\n  char *str;\n  void *func;\n} SYMBOL;\n\nSYMBOL standTbl[] = \n{\n";
while ($line = <IN>)
{
  @col = split(/ /, $line);
  chomp($col[2]);
  if (($col[1] eq "T") && ($col[2] !~ /\./) && $funcs++ ne "%")
  {
    print C "  {\"$col[2]\", (void *)$col[2]},\n"; 
    print H "extern void $col[2](void);\n";
  }
}
print C "};\n\nunsigned long standTblSize = $funcs;\n";

close C;
close H;
close IN;
