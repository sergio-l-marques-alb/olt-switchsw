# contentdepend.pl <find arguments>
#
# formats find <arguments> for use in makefile dependencies

$arg = shift(@ARGV);

$line = <>;
while ($line) {
   chomp($line);
   $line =~ s/[\00-\31]$//g;

   $nextline = <>;

   if (lc($arg) eq  '-first')
   {
      print "$line ";
      print "\\";
   }
   elsif (lc($arg) eq  '-target')
   {
      print "$line:";
   }
   elsif ($nextline eq '' && !(lc($arg) eq  '-notlast' ||  lc($arg) eq '-none'))
   {
      print "\t$line ";
      print "\\";
   }
   elsif ($nextline ne '' && lc($arg) ne '-none')
   {
      print "\t$line ";
      print "\\";
   }
   else
   {
      print "\t$line ";
   }

   print "\n";

   $line = $nextline;
}

