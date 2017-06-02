#! /usr/bin/perl

@tokens = split /[.]/, $ARGV[0];
$filename = $tokens[0];

while (<STDIN>) {
   s/$filename.o:/$filename.o $filename.d:/;
   print $_;

}

