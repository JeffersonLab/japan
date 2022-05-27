#!/usr/bin/env perl
use strict;
use warnings;
use Getopt::Long;
use File::Glob qw[:glob];

################################################################
### qwcanonify
### Rob Mahurin rob@jlab.org 2010-10-07
### search for "helpstring" below for documentation
################################################################

my ($help,$quiet,$dryrun,$runrange); 
my @searchpaths = (`pwd`, $ENV{QW_DATA});

## Add other useful defalut paths here, whitespace-separated
push @searchpaths, qw[
	/home/cdaq/qweak/QwScratch
	/cache/mss/hallc/qweak/raw
  ];

my $optstatus = GetOptions
  "help|h|?"	=> \$help,
  "quiet|q"	=> \$quiet,
  "dryrun"	=> \$dryrun,
  "runrange=s"	=> \$runrange,
;

## If there are any arguments left, search those paths INSTEAD of the defaults
if (@ARGV) { @searchpaths = @ARGV; }

### Search only existing paths, and each only once. From perlfaq4.
my %seen = ();
chomp(my @orig = @searchpaths);
@searchpaths = grep {-d} grep { !$seen{$_}++ } @orig;

my $helpstring = <<EOF;
qwcanonify: create analyzer-friendly symlinks to data files
Usage: qwcanonify -r <runrange> [/optional/search/path]
Required argument:
	-r runrange	run number(s) to look for
Options:
	--help		print this stuff
	--quiet		just do things, don't yak about it
	--dryrun	just yak about it, don't do anything
Symlinks are made in \$QW_DATA; yours is
	$ENV{QW_DATA}
Your search path is 
EOF
$helpstring .= "\t$_\n" foreach @searchpaths;

die $helpstring
  if $help or !$optstatus;
die "No run range specified\n"
  unless defined $runrange;
die "$helpstring\nerror: QW_DATA not defined!\n"
  unless exists $ENV{QW_DATA};
die "$helpstring\nerror: None of the search paths exist!\n"
  unless @searchpaths;

### Parse the run numbers
my ($lowrun,$highrun,$garbage) = split /:/, $runrange;
if (!defined $highrun) {
  $highrun = $lowrun;
}

die "run range '$runrange' contains garbage\n"
  if $runrange =~ /[^\d:]/ or defined $garbage;

################################################################
### okay, the code is documented and error-checked, now do something
################################################################

RUN: foreach my $run ($lowrun .. $highrun) {
  my $dest = "$ENV{QW_DATA}/QwRun_$run.log";
  my @old_dest = glob "$dest*";
  if (@old_dest) {
    warn "skipping existing $dest*\n" unless $quiet;
    next RUN;
  }
  PATH: foreach my $path (@searchpaths) {
    ## I don't actually know that any CODA output would end in .log,
    ## and if both .dat and .log files exist for a run then the
    ## behavior here is not well-defined.
    my @found = glob "$path/*_$run.{dat,log}*";

    FILE: foreach my $src (@found) {
      my($segment) = ($src =~ /(\.\d+)?$/); 
      $segment = "" unless defined $segment;
      print "$src -> $dest$segment\n" unless $quiet;
      unless ($dryrun) {
	symlink "$src", "$dest$segment"
	  or die "symlink failed: $!\n";
      }
    }
    next RUN if @found;
  }
  warn "run $run not found\n" unless $quiet;
}
