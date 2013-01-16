
use strict;
use Switch;
use XML::Twig;
use Data::Dumper;
use File::Compare;

my @srcs     = @ARGV;

sub move_elements
{
  my ($copy, $file, $type) = @_;

  my $t = new XML::Twig(pretty_print => 'indented', empty_tags => 'normal', comments => 'keep');
  $t->parsefile($file);

  my $dtypeEnt    = undef;
  my @dtypes = $t->findnodes("/FastPathXML/data-types");
  if ($#dtypes < 0)
  {
    $dtypeEnt = $t->root->new('data-types')->paste($t->root);
    $dtypeEnt->set_att(version => '1.0');
  }
  else
  {
    $dtypeEnt    = $dtypes[$#dtypes];
  }

  foreach my $oldEnt ($t->findnodes("/FastPathXML/$type"))
  {
    my $oldEnt = $oldEnt->cut;
    $oldEnt->paste(last_child => $dtypeEnt);
  }

  unlink("$file.out");
  open(OUT, ">$file.out") or die "cannot open out file $file:$!";
  $t->print(\*OUT);
  close(OUT);

  if (compare($file, "$file.out") != 0)
  {
    system("cp -f $file.out $file\n");
  }
  unlink("$file.out");
}

sub change_xml
{
  my ($dir, $type) = @_;

  open(allXMLS, "find $dir/  -name '*-$type.xml' -print|sort|");
  while (my $file = <allXMLS>)
  {
    print $file;
    chomp($file);

    my $copy = new XML::Twig(pretty_print => 'indented', empty_tags => 'normal', comments => 'keep');
    $copy->parsefile($file);

    my $out = "";
    open(FILE, "< $file") or die "Can't open $file : $!";
    while (<FILE>)
    {
      $out .= $_;
    }
    close FILE;

    $out =~ s/misc\.dtd/types.dtd/g;
    ##$file = $file . ".1";

    unlink($file);
    open(FILE, "> $file") or die "Can't open $file : $!";
    print FILE $out;
    close FILE;

    move_elements($copy, $file, "usrtype");
    move_elements($copy, $file, "type");
    move_elements($copy, $file, "enum");
    move_elements($copy, $file, "row-status");
  }
}

foreach my $dir (@srcs)
{
  change_xml($dir, "usrtype");
  change_xml($dir, "type");
  change_xml($dir, "enum");
  change_xml($dir, "row_status");
}

