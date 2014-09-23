#!/usr/bin/perl -w
use strict;

if ($#ARGV != 2) {
    die "Usage: $0 <cross-nm> <executable> <output file>\n";
}
my $nm = $ARGV[0] || '';
my $exe = $ARGV[1] || '';
my $output = $ARGV[2] || '';
if (($nm eq '') || ($exe eq '') || ($output eq '')) {
    die "Usage: $0 <cross-nm> <executable> <output file>\n";
}
if (! -x $nm && ! -x "$nm.exe") {
    die "$nm not executable\n";
}
if (! -s $exe) {
    die "$exe does not exist or is empty\n";
}
# Command: Only global symbols, no undefined symbols, sort by address
my $in_command = $nm . ' --defined-only -g -n ' . $exe;
my $out_command = '| gzip -9 > ' . $output;
if ($output !~ /\.gz$/) {
    $out_command = '> ' . $output;
}
if (!open(INPUT, $in_command . ' |')) {
    die "Error running nm [command $in_command]: $!\n";
}
if (!open(OUTPUT, $out_command)) {
    die "Error opening output gzip pipe or file $output: $!\n";
}
my $biggest_addr = '0';
while (my $cur_line = <INPUT>) {
    chomp($cur_line);
    my ($addr, $type, $symbol) = split(/ /, $cur_line);
    next if ($type ne 'T');
    next if ((length($addr) > 8) || ($addr =~ /[^0-9a-fA-F]/));
    unless ($addr lt $biggest_addr) {
	$biggest_addr = $addr;
    }
    next if ($symbol =~ /^\_/);
    while (length($addr) < 8) {
	$addr = '0' . $addr;
    }
    my $trunc_sym = $symbol;
    if (length($symbol) > 63) {
	$trunc_sym = substr($symbol, 0, 63);
	print "Warning: Symbol $symbol too long, truncated to $trunc_sym for devshell purposes\n";
    }
    print OUTPUT $addr . $trunc_sym . "\n"; # Intentionally no space between them
}
# Need a last line, for function->address lookup in osapi
print OUTPUT $biggest_addr . "END_OF_PROGRAM\n";
if (!close(OUTPUT)) {
    die "Error closing output pipe: $!\n";
}
if (!close(INPUT)) {
    die "Error closing input pipe: $!\n";
}
exit 0;
