#!/usr/bin/perl

#
# Checks the /tmp/pgen/req-<version>-<start>-<count> files at regular
# intervals and generate planets accordingly.
#
# Syntax : ./planetmaker.pl
#

$outDir = "/mnt/planets";

close(STDIN);
close(STDERR);
close(STDOUT);
if	(fork())
{
	exit(0);
}

chdir("/opt/planetgen");
mkdir("/tmp/pgen", 01777) if (! -d "/tmp/pgen");
while	(1)
{
	chop($find = `find /tmp/pgen/ -name 'req-*-*-*'`);
	if	($find ne "")
	{
		@list = split /\n/, $find;

		foreach my $name (@list)
		{
			unlink($name);
			my ($junk,$version,$first,$count) = split /-/, $name;

			for	($i=$first;$i<$first+$count;$i++)
			{
				my	$fn = "/tmp/pgen/out.pov";
				`./generate.pl $fn`;
				`povray +W80 +H80 +O$outDir/$version/l/$i.png -D -V $fn >/dev/null 2>&1`;
				`povray +W30 +H30 +O$outDir/$version/s/$i.png -D -V $fn >/dev/null 2>&1`;
				unlink($fn);
			}
		}
	}
	sleep(60);
}
