#
open (INFILE,"symTbl.tmp")|| die "Can't open symTbl.tmp";
open (OUTFILE,">symTbl.c");

while (<INFILE>)
{
   if ((-1 != index($_,"default_alloc_template")) ||
       (-1 != index($_,"SYM_DATA")) ||
       (-1 != index($_,"SYM_BSS")))
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
         if (-1 != index($_, "SYMBOL standTbl [" )) {
            /(\d+)/;
            print OUTFILE "SYMBOL standTbl [] = \n";
         }
         else {
            if (-1 != index($_, "ULONG standTblSize = " )) {
               print OUTFILE "ULONG standTblSize = sizeof(standTbl) / sizeof(SYMBOL);", "\n";
            }
            else
            {
                print OUTFILE $_;
            }
         }
       }
   }
}

close INFILE;
close OUTFILE;
