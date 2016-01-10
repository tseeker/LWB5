#!/usr/bin/perl

#
# Planet Generator
#
# Syntax: ./generate.pl <output>
#
# Generates a random POVRay script for a planet
#


sub	genSingle
{
	# Load template
	open(TMPL, "<template.pov") or die "couldn't open template\n";
	@tmpl = <TMPL>;
	close TMPL;

	# Generate texture
	$pSize = sprintf("%.2f", 1 + rand() * 0.75);
	@types = ("agate", "bozo", "granite", "wood");
	$type = $types[int(rand() * @types)];
	$ambient = rand() * .25;
	$shining = rand() * .1;
	$txt = "pigment {\n\t\t\t$type\n";

	# Generate turbulence
	@tm = (int(rand() * 3), int(rand() * 3), int(rand() * 3));
	$toc = 1 + int(rand() * 3);
	$tol = sprintf "%.2f", 1 + rand() * 2;
	$too = sprintf "%.2f", 0.25 + rand() * 0.5;
	$txt .= "\t\t\twarp { turbulence <" . join(",", @tm) . "> octaves $too lambda $tol omega $too }\n";

	# Generate color map
	$mainColor = int(rand() * 3);
	$secColor = int(rand() * 3);
	$nColors = int(rand() * 10) + 5;
	$current = 0;
	$txt .= "\t\t\tcolor_map {\n";
	for	($i=0;$i<$nColors;$i++)
	{
		$mx = (1 - $current) / ($nColors - $i);
		$cs = sprintf "%.2f", ($i == $nColors - 1) ? (1 - $current) : (rand() * $mx);
		$cs += $current;
		$txt .= "\t\t\t\t[$cs color rgb <";
		@cg = map { sprintf "%.2f", $_ } (0.4 + rand() * 0.5, 0.2 + rand() * 0.5, rand() * 0.5);
		@rc = ();
		for	($j=0;$j<3;$j++)
		{
			$rc[$j] = ($mainColor == $j ? $cg[0] : ($secColor == $j ? $cg[1] : $cg[2]));
		}
		$txt .= join(",",@rc) . ">]\n";
		$current = $cs;
		
	}
	$txt .= "\t\t\t}\n";

	# Random pigment rotation
	$txt .= "\t\t\trotate x * " . int(rand() * 360) . "\n";
	$txt .= "\t\t\trotate y * " . int(rand() * 360) . "\n";
	$txt .= "\t\t\trotate z * " . int(rand() * 360) . "\n";
	$txt .= "\t\t}\n";

	# Generate finish
	$txt .= "\t\tfinish { ambient $ambient phong $shining }\n";

	# Light source rotation
	$lsr = int(rand() * 358);

	open(TARGET, ">$ref") or die "couldn't create target file\n";

	# Rings
	if	($pSize < 1.25 && rand() < 0.5)
	{
		print TARGET "#declare RINGS = 1;\n";
		$rhrad = $pSize + 0.05 + sprintf("%.2f", rand() * 0.2);
		$rrad = $rhrad + 0.3 + sprintf("%.2f", rand() * 0.2);
		$t = rand();
		$b = 0.6 + rand() * 0.2;
		if	($t < 0.25)
		{
			@cg = map { sprintf "%.2f", $_ } ($b + rand() * 0.1, $b + rand() * 0.05, $b);
			@rc = ();
			for	($j=0;$j<3;$j++)
			{
				$rc[$j] = ($mainColor == $j ? $cg[0] : ($secColor == $j ? $cg[1] : $cg[2]));
			}
			($rr, $rg, $rb, $rt) = (@rc,sprintf("%.2f",0.2+rand()*0.15));
		}
		else
		{
			($rr, $rg, $rb, $rt) = map { sprintf "%.2f", $_ } ($b,$b,$b,0.2+rand()*0.15);
		}

		$rcmap  = "\t\t\t\t[0.00 color rgbt <$rr,$rg,$rb,1>]\n";
		$rcmap .= "\t\t\t\t[0.05 color rgbt <$rr,$rg,$rb,$rt>]";
		$acc = 0;
		$n = 0.05;
		$oldlv = 1;
		while	($n < 0.99)
		{
			$n += 0.05;
			$acc += rand();
			next if $acc < (.5/($rrad-$rhrad));
			$acc = 0;

			do { $lv = 1 + int(rand() * 3); } while ($lv == $oldlv);
			$oldlv = $lv;
			$lv *= $rt;
			($lv > 1) && ($lv = 1);
			$rcmap .= "\n\t\t\t\t[" . sprintf("%.2f", $n) . " color rgbt <$rr,$rg,$rb,$lv>]";
		}
		$rcmap .= "\t\t\t\t[1.00 color rgbt <$rr,$rg,$rb,1>]\n";

		do { $rrot = int(rand() * 60) - 30; } while (abs($rrot) < 10);
		do { $rrot2 = int(rand() * 60) - 30; } while (abs($rrot2) < 10);
	}

	foreach $l (@tmpl)
	{
		$l =~ s/PSIZE/$pSize/;
		$l =~ s/TEXTURE/$txt/;
		$l =~ s/LSROT/$lsr/;
		$l =~ s/RRAD/$rrad/;
		$l =~ s/RHRAD/$rhrad/;
		$l =~ s/RROTZ/$rrot/;
		$l =~ s/RROTY/$rrot2/;
		$l =~ s/RR/$rr/;
		$l =~ s/RG/$rg/;
		$l =~ s/RB/$rb/;
		$l =~ s/RT/$rt/;
		$l =~ s/RCMAP/$rcmap/;
		print TARGET $l;
	}
	close TARGET;
}


sub	genCluster
{
	# Load template
	open(TMPL, "<template2.pov") or die "couldn't open template\n";
	@tmpl = <TMPL>;
	close TMPL;

	$nPlanets = 2 + int(rand() * 2);
	$psBase = 1.2 / $nPlanets;
	$psRand = 0.55 / $nPlanets;
	$mainColor = int(rand() * 3);
	@ps = @tx = @cx = @cy = @cz = ();
	for	($p=0;$p<$nPlanets;$p++)
	{
		$pSize = sprintf("%.2f", $psBase + rand() * $psRand);

		# Generate texture
		@types = ("agate", "bozo", "granite", "wood");
		$type = $types[int(rand() * @types)];
		$ambient = rand() * .25;
		$shining = rand() * .1;
		$txt = "pigment {\n\t\t\t$type\n";

		# Generate turbulence
		@tm = (int(rand() * 3), int(rand() * 3), int(rand() * 3));
		$toc = 1 + int(rand() * 3);
		$tol = sprintf "%.2f", 1 + rand() * 2;
		$too = sprintf "%.2f", 0.25 + rand() * 0.5;
		$txt .= "\t\t\twarp { turbulence <" . join(",", @tm) . "> octaves $too lambda $tol omega $too }\n";

		# Generate color map
		$secColor = int(rand() * 3);
		$nColors = int(rand() * 10) + 5;
		$current = 0;
		$txt .= "\t\t\tcolor_map {\n";
		for	($i=0;$i<$nColors;$i++)
		{
			$mx = (1 - $current) / ($nColors - $i);
			$cs = sprintf "%.2f", ($i == $nColors - 1) ? (1 - $current) : (rand() * $mx);
			$cs += $current;
			$txt .= "\t\t\t\t[$cs color rgb <";
			@cg = map { sprintf "%.2f", $_ } (0.4 + rand() * 0.5, 0.2 + rand() * 0.5, rand() * 0.5);
			@rc = ();
			for	($j=0;$j<3;$j++)
			{
				$rc[$j] = ($mainColor == $j ? $cg[0] : ($secColor == $j ? $cg[1] : $cg[2]));
			}
			$txt .= join(",",@rc) . ">]\n";
			$current = $cs;
			
		}
		$txt .= "\t\t\t}\n";

		# Random pigment rotation
		$txt .= "\t\t\trotate x * " . int(rand() * 360) . "\n";
		$txt .= "\t\t\trotate y * " . int(rand() * 360) . "\n";
		$txt .= "\t\t\trotate z * " . int(rand() * 360) . "\n";
		$txt .= "\t\t}\n";

		# Generate finish
		$txt .= "\t\tfinish { ambient $ambient phong $shining }\n";

		push @ps, $pSize;
		push @tx, $txt;
	}

	# Generate coordinates
	if	($nPlanets == 2)
	{
		$e = 1.5 - ($ps[0] + $ps[1]);
		$w = ($e + rand() * $e) / 2;
		@cx = ($w+$ps[0], -$w-$ps[1]);
		@cy = @cz = (0, 0);
	}
	else
	{
		$e1 = $ps[0] + $ps[1];
		$e2 = $ps[2] + $ps[1];
		$e3 = $ps[0] + $ps[2];
		$m = (($e1>$e2 && $e1>$e3) ? $e1 : (($e2>$e1 && $e2>$e3) ? $e2 : $e3));
		$e = 1.3 - $m;
		$w = ($e + rand() * $e) / 2;
		$rw = $w + $m;
		$d = $rw * cos(3.1415926535/6);
		@cx = ($d, $d * cos(2*3.1415926535/3), $d * cos(4*3.1415926535/3));
		@cy = (0, $d * sin(2*3.1415926535/3), $d * sin(4*3.1415926535/3));
		@cz = (0, 0, 0);
	}
	$rotx = int(rand() * 360);
	$roty = int(rand() * 360);
	$rotz = int(rand() * 360);

	print join(',', @cx) . "\n";
	print join(',', @cy) . "\n";

	# Light source rotation
	$lsr = int(rand() * 358);

	open(TARGET, ">$ref") or die "couldn't create target file\n";
	foreach $l (@tmpl)
	{
		$l =~ s/LSROT/$lsr/;
		$l =~ s/ROTX/$rotx/;
		$l =~ s/ROTY/$roty/;
		$l =~ s/ROTZ/$rotz/;
		$l =~ s/NP;/$nPlanets;/;
		for	($i=0;$i<$nPlanets;$i++)
		{
			$pSize = $ps[$i];
			$txt = $tx[$i];
			$x = $cx[$i];
			$y = $cy[$i];
			$z = $cz[$i];
			$np = $i + 1;
			$l =~ s/PSIZE$np/$pSize/;
			$l =~ s/TEXTURE$np/$txt/;
			$l =~ s/X$np/$x/;
			$l =~ s/Y$np/$y/;
			$l =~ s/Z$np/$z/;
		}
		print TARGET $l;
	}
	close TARGET;
}



$ref = $ARGV[0];
die "Syntax: $0 <output>\n" if $ref eq "";

if (rand() < 0.9)	{ genSingle(); }
else			{ genCluster(); }
