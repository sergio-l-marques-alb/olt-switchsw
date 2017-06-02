
use strict;
use warnings;

my $nmpath  = shift(@ARGV);
my $cfile   = shift(@ARGV);
my $dbgfile = shift(@ARGV);
my $fpout   = shift(@ARGV);
my $iplDir  = shift(@ARGV);
my $libFile = shift(@ARGV);

my $varExtern = "";
my $varEntry  = "";
my $funcs     = 0;
my $debug_show = 0;
my %allSym    = ();

sub read_object_files
{
  my ($curDir) = @_;

  if (open(INLIST, "find $curDir -name '*.o' -print|sort|"))
  {
    while (my $file = <INLIST>)
    {
      chomp($file);
      next unless (open(IN, "$nmpath $file | grep \" T \"|"));
      while (my $line = <IN>)
      {
        chomp($line);
        my @col = split(/ /, $line);
        $allSym{$col[2]} = 1;
      }
      close(IN);
    }
    close(INLIST);
  }
}

sub read_lib_file
{
  my ($file) = @_;

  return unless (open(IN, "$nmpath $file | grep \" T \"|"));
  while (my $line = <IN>)
  {
    chomp($line);
    my @col = split(/ /, $line);
    $allSym{$col[2]} = 1;
  }
  close(IN);
}

open(DBG_IN, $dbgfile) || exit 0;

read_object_files $fpout;
read_object_files $iplDir;
read_lib_file $libFile;

while (my $line = <DBG_IN>)
{
  chomp($line);
  next if ("$line" eq "");
  my @col = split(/ /, $line);
  next if ("$col[0]" eq "" || "$col[1]" eq "" || $funcs++ eq "%");
  if (defined($allSym{$col[1]}))
  {
    $varEntry  = $varEntry . "  {\"$col[0]\", (void *)$col[1]},\n";
    $varExtern = $varExtern . "extern void $col[1](void);\n";
  }
  else
  {
    print "Skipping $col[0] as $col[1] it is not present in the build\n";
    $funcs--;
  }
}
close(DBG_IN);

# Collect all the debug and show functions
if($debug_show eq 1)
{
  foreach my $sym (keys %allSym)
  {
    next unless ($sym =~ m/Debug/g || $sym =~ m/Show/g);
    $varEntry  = $varEntry . "  {\"$sym\", (void *)$sym},\n";
    $varExtern = $varExtern . "extern void $sym(void);\n";
    $funcs++;
  }
}

open(C, ">$cfile") || die "Can't open '$cfile': $!\n";
print C "typedef struct\n{\n  char *str;\n  void *func;\n} SYMBOL;\n";
print C $varExtern;
print C "\nSYMBOL standTbl[] = \n{\n";
print C $varEntry;
print C "  {(char *)0, (void *)0}\n};\n";
print C "\nunsigned long standTblSize = $funcs;\n";
close(C);
