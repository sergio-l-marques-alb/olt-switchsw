#
open (INFILE,"symTbl.tmp")|| die "Can't open symTbl.tmp";
open (OUTFILE,">symTbl.c");

while (<INFILE>)
{
   if (-1 != index($_,"default_alloc_template"))
   {
   }
   else
   {
      if (-1 != index($_,"vxWorks.h"))
      {
         print OUTFILE "#include <lvl7_vxWorks.h>\n";
      }
      else
      {
         print OUTFILE $_;
      }
   }
}

close INFILE;
close OUTFILE;
