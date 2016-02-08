#!/usr/bin/perl
use strict;

# Outputs a file with a list of all .lib files in the given directory, 
# sorted by size, smallest to largest. 
if ((scalar(@ARGV) != 2) || (! -d $ARGV[0])) {
    die "Usage: $0 libs_directory output_filename\n";
}
my $dir = $ARGV[0];
my $outfile = $ARGV[1];
$dir =~ s/\/$//g;
if (!opendir(LIBSDIR, $dir)) {
    die "Can't open libs directory $dir: $!\n";
}
if (!open(OUTFILE, '>', $outfile)) {
    die "Can't open output file $outfile: $!\n";
}
my %libs = ();
foreach my $entry (readdir(LIBSDIR)) {
    next if ($entry !~ /\.lib$/);
    my $fname = $dir . '/' . $entry;
    $libs{-s $fname} = $fname;
}
# At this point, %libs has all of the .lib files, keys = the file sizes, 
# data = the file paths. 
closedir(LIBSDIR);
foreach my $size (sort({$a<=>$b} keys(%libs))) {
    print OUTFILE $libs{$size} . "\n";
}
close(OUTFILE);
exit 0;
