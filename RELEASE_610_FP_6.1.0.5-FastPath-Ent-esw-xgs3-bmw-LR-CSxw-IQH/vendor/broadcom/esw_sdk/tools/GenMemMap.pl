#!/usr/local/bin/perl
#
# Generate an include file from a memory map.
# This could all be done with the C-preprocessor
# but this would make debugging a bit more
# tedious.
# 
# usage:
#
#   ./GenMemMap.pl in_file_name out_file_name
#
#################################################
use File::Basename;

if (@ARGV != 2) {
    print "usage:\n";
    print "./GenMemMap.pl in_file_name out_file_name\n";
    exit 1;
}

# remove whitespace
sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

# ------------------------
# fix numbers, support:
#     64k  kilo 
#     12m  mega 
#    0x12  hex  
# ------------------------
sub fixnum($) 
{
    my $pow = 1;
    $_ = shift;

    if (/(.*)[kK]+/) {
	$pow = $pow * 1024;
	s/[kK]+//;
    }
    
    if (/(.*)[mM]+/) {
	$pow = $pow * 1024 * 1024;
	s/[mM]+//;
    }

    if (/0x/) {
	$_ = hex($_);
    }
    
    return $_ * $pow;
}

# for text alignment
my $max_pad_len = 30;

# blank string of n spaces
sub padn($)
{
    $length = shift;

    $padlen = $max_pad_len - $length;

    # if you hit this, increase max_pad_len
    if ($length < 0) {
	print "Error, fix script to allow larger padding length\n";
	exit 1;
    }

    $pad = "";
    for ($i = 0; $i < $padlen; $i = $i + 1) {
	$pad = $pad . " ";
    } 
    
    return $pad;
}

# for subsequent compatibility
my $script_version = 1;

my %sizes;
my %descs;
my %mems = ();
my @includes = ();
my @order = ();

$infile_name = $ARGV[0];
$outfile_name = $ARGV[1];

open (F_IN, $infile_name) or die "Open $infile_name failed ...";
open (F_OUT, ">$outfile_name.tmp") or die "Open $outfile_name.tmp failed ...";

# read in and parse lines
$curr_line = 0;
while ( <F_IN> ) {
    # get line
    chomp;

    $curr_line = $curr_line + 1;

    if (/^#/ || /^$/) {	
	next;
    }
    if (/^macro_prefix\s*=\s*(.*)/) {
	$macro_prefix = $1;
	next;
    }
    if (/^version\s*=\s*(.*)/) {
	$vers = $1;
	next;
    }
    if (/^include\s+(.*)/) {
	push @includes, $1;
	next;
    }
    if (/^desc\s+(\".*\")/) {
	$desc = $1;
	next;
    }
    if (/^mem\s+(.+)\s+(.+)/) {
	$mem_name = trim($1);
	$mem_size = trim($2);
	$sizes{$mem_name} = trim($2);
	$descs{$mem_name} = $desc;
	push @order, trim($1);
	next;
    }
    if (/^tbl\s+(.*)\s+(.*)/) {
	$tbl_name = trim($1);
	$tbl_size = trim($2);
	push @{$mems{$mem_name}}, "$tbl_name $tbl_size";
	next;
    }

    print "Syntax Error\n";
    print "$infile_name:$curr_line: $_\n";
    exit 1;
}

close F_IN;

# Start dumping out to the file
# first the copyright notice

print F_OUT "/* -*- mode:c++; c-style:k&r; c-basic-offset:2; indent-tabs-mode: nil; -*-\n";
print F_OUT " * vi:set expandtab cindent shiftwidth=2 cinoptions=\:0l1(0t0g0:\n";
print F_OUT " *\n";
print F_OUT " * Proprietary information of:\n";
print F_OUT " *\n";
print F_OUT " * Broadcom Corporation\n";
print F_OUT " * 600 Federal Street, \n";
print F_OUT " * Andover, Ma. 01810\n";
print F_OUT " *\n";
print F_OUT " * Copyright (c) 2002-2006, Broadcom Corporation. All Rights Reserved\n";
print F_OUT " *\n";
print F_OUT " * This licensed source code contains confidential and proprietary\n";
print F_OUT " * information of Broadcom Corporation, is the property of Broadcom\n";
print F_OUT " * Corporation or its licensors and may not be copied, reproduced,\n";
print F_OUT " * distributed, licensed, sub-licensed, displayed, modified, performed\n";
print F_OUT " * or otherwise disclosed or used without the express written consent\n";
print F_OUT " * of Broadcom Corporation.\n";
print F_OUT " *\n";
print F_OUT " * the source code is provided 'as is' without warranties of any kind,\n";
print F_OUT " * express or implied, including without limitation any warranty as to\n";
print F_OUT " * non-infringement or the implied warranties of merchantability or\n";
print F_OUT " * fitness for a particular purpose.\n";
print F_OUT " *\n";
print F_OUT " * $outfile_name : AUTO-GENERATED Memory Map from $infile_name\n";
print F_OUT " *\n";
print F_OUT " * --------------------------------------------------------------------------*/\n";

# then the includes
foreach $include (@includes) {
    print F_OUT "#include \"$include\"\n";
}
print F_OUT "\n";

# then comments about the tables
foreach $mem (@order) {
    print F_OUT "/*\n";
    print F_OUT " * Memory : $mem\n";
    print F_OUT " * Size   : $sizes{$mem}\n";
    print F_OUT " *\n";

    $addr = 0;
    foreach $tbl (@{$mems{$mem}}) {
	$_ = $tbl;
	m/(.*)\s+(.*)/;
	$name = trim($1);
	$size = trim($2);
	$hsize = trim($2);

	# support mem@offset notation
	if ($size =~ /(.*)@(.*)/) {
	    $hsize = $1;
	    $size = fixnum($1);
	    $fixed = fixnum($2);
	    if ($addr > $fixed) {
		print "Error - Memory already defined at fixed location\n";
		exit 1;
	    }
	    $addr = $fixed;
	}

	# short-hand number into decimal	
	$size = fixnum($size);

	# trim, and then pad to align
	$name = trim($name);
	$pad  = padn(length($name));

	print F_OUT " * $name " . $pad . sprintf("0x%08x",$addr) . " -> " . 
	    sprintf("0x%08x",$addr+$size) . "    size $hsize\n";

	$addr = $addr + $size;

	if ($addr > fixnum($sizes{$mem})) {
	    print "Error, exceeded size of memory = $sizes{$mem}\n";
	    exit 1;
	}

    }
    print F_OUT " */\n\n";
}
print F_OUT "\n";

# finally create the defines
foreach $mem (@order) {
    $addr = 0;
    foreach $tbl (@{$mems{$mem}}) {
	$_ = $tbl;
	m/(.*)\s+(.*)/;
	$name = trim($1);
	$size = trim($2);
	
	# support mem@offset notation
	if ($size =~ /(.*)@(.*)/) {
	    $size = fixnum($1);
	    $fixed = fixnum($2);
	    if ($addr > $fixed) {
		print "Error - Memory already defined at fixed location\n";
		exit 1;
	    }
	    $addr = $fixed;
	}

	# short-hand number into decimal
	$size = fixnum($size);

	# trim, and then pad to align
	$name = trim($name);
	$pad  = padn(length($name));

	$prelude = "#define $macro_prefix" . trim(uc($name));
	print F_OUT $prelude . "_BANK  " . $pad . $mem . "\n";
	print F_OUT $prelude . "_BASE  " . $pad . sprintf("0x%08x\n", $addr);
	print F_OUT $prelude . "_SIZE  " . $pad . sprintf("0x%08x\n", $size);
	print F_OUT "\n";
	
	$addr = $addr + $size;

	if ($addr > fixnum($sizes{$mem})) {
	    print "Error, exceeded size of memory = $sizes{$mem}\n";
	    exit 1;
	}
    }    
}

close F_OUT;

# only on success, does it get renamed
system("mv $outfile_name.tmp $outfile_name");

exit 0;

