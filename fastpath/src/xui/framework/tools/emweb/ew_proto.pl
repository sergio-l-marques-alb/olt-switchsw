#!/usr/bin/perl

use strict;

# Usage: ew_proto.pl <outdir> <path to ew_proto.h>

my %split_files;
my %ewproto_file_handles   = ();
my $current_ewproto_suffix = "common";
$split_files{$current_ewproto_suffix} = [];

## start process
my $outdir  = ( defined $ARGV[0] ? $ARGV[0] : "." );
my $infile = ( defined $ARGV[1] ? $ARGV[1] : "-" );

&ew_proto_split($infile);
&ew_proto_print_files();

## Split ewproto header file
sub ew_proto_split($)
{
  my $infile = shift;
  open( EWPROTO_HDRIN, "$infile" ) || die "$0: Can't open $infile: $!\n";

  my $lineno = 0;
  while (my $line = <EWPROTO_HDRIN>)
  {
    $lineno++;
    if ( $line =~ m/to process the form from file ([^\/]*)\.html/ )
    {
      $current_ewproto_suffix = $1;
      $current_ewproto_suffix =~ s#/#_#g;
      $split_files{$current_ewproto_suffix} = [];
    }
    elsif ( $line =~ m/The following structure was created using the form from file/ )
    {
      my $line2 = <EWPROTO_HDRIN>;
      $lineno++;
      push @{$split_files{$current_ewproto_suffix}}, "*/\n";    ## close the comment in current file
      if ( $line2 =~ m/\/([^\/]*)\.html starting on line/ )
      {
        $current_ewproto_suffix = $1;
        $current_ewproto_suffix =~ s#/#_#g;
        push @{ $split_files{$current_ewproto_suffix} }, "/*\n";
        push @{ $split_files{$current_ewproto_suffix} }, $line;
        push @{ $split_files{$current_ewproto_suffix} }, $line2;
      }
      else
      {
        die "$infile($lineno): not expected $line2\n";
      }
    }
    elsif ( $line =~ m/#endif/ )
    {
      push @{$split_files{common}}, $line;
    }
    else
    {
       push @{$split_files{$current_ewproto_suffix}}, $line;
    }
  }

## close the input file
  close(EWPROTO_HDRIN);
}

sub ew_proto_print_files() {
## print all split files
  foreach my $file ( sort(keys %split_files) )
  {
    my $outfile = "$outdir/ew_proto_${file}.h";
    my $newdata = join('', @{$split_files{$file}});
    if (-f $outfile && open(OUTFILE, "<$outfile")) {
       my @currentdata = <OUTFILE>;
       my $currentdata = join('',@currentdata);
       close(OUTFILE);
       next if ($currentdata eq $newdata);
    }
    if (open( OUTFILE, ">$outfile" ) or die "$0: Can't open $outfile: $!\n") {
       print OUTFILE $newdata;
       close(OUTFILE);
    }
  }
}

