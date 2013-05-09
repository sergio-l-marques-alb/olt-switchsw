
my $fp_base  = shift(@ARGV);
my $dst_dir  = shift(@ARGV);
my $mak_file = shift(@ARGV);
my $sep      = shift(@ARGV);
my $optFile  = shift(@ARGV);

my %srcs = ();
open(FILE, $optFile) || ((warn "Can't open file $optFile\n"), next FILE);
while (<FILE>)
{
  my ($opt, $line) = split(/$sep/, $_);
  $opt =~ s/^\s*(\S*(?:\s+\S+)*)\s*$/$1/;
  if ($opt eq "XML_COMMON" || $opt eq "XML_PRODUCT")
  {
    foreach (split(/ /, $line))
    {
      next if ($_ eq "");
      next if (!-d "$fp_base/$_");
      $srcs{"$fp_base/$_"} = 1;
    }
  }
}
close(FILE);

my $types_str      = "";
my $depend_genfile = "$dst_dir/xe_depend.gen";
my $depend_tmpfile = "$dst_dir/xe_depend.tmp";

$types_str = $types_str . "*.tmpl ";
$types_str = $types_str . "*.html *.xml ";
$types_str = $types_str . "*.css *.js ";
$types_str = $types_str . "*.gif *.jpg ";
$types_str = $types_str . "*.jar _ACCESS ";

open(D_FILES, ">$depend_tmpfile") || die "Couldn't open $depend_tmpfile to write\n";
for my $type (split(/ /, $types_str))
{
  for my $dir (keys %srcs)
  {
    open(allFiles, "find $dir -maxdepth 1 -name '$type' -print|sort|");
    while ($file = <allFiles>)
    {
      print D_FILES "XE_DEPEND_FILES += $file";
    }
    close(allFiles);
  }
}
print D_FILES "XE_DEPEND_FILES += $depend_genfile\n";
print D_FILES "export FP_ROOT = $fp_base\n";
print D_FILES "include $mak_file\n";
close(D_FILES);

if (-f $depend_genfile) {
  $cmp = system("cmp $depend_genfile $depend_tmpfile") / 256;
} else {
  $cmp = 1;
}

if ($cmp ne '0')
{
  `cp -f $depend_tmpfile $depend_genfile`;
  `rm -f xml_rebuild.done`;
}
