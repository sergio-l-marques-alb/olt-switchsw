#! /usr/bin/perl

@tokens = split /[.]/, $ARGV[1];
$filename = $tokens[0];

print "$ARGV[0]/";

while (<STDIN>) {
   s/$filename.o:/$filename.o $filename.d:/;
   print $_;

}

