#!/usr/bin/perl

######################################################################
# LegacyWorlds Beta 5
# System control script
#
# Without arguments:
#   should run as root; creates a FIFO from which it reads commands.
#
# With arguments:
#   writes arguments to the pipe
######################################################################


###
## Configuration
#

$fifoPath  = "/tmp/.lwFifo";
$ctrlPath  = "/tmp/.lwControl";
$fsUser    = 0;
$fsGroup   = 33;




###
## Main code below
#

use IO::File;
require POSIX;

# If we have arguments, write to the FIFO and exit
if (@ARGV > 0) {
	if ($ARGV[0] eq '--start') {
		&start();
	} elsif ($ARGV[0] eq '--stop') {
		&stop();
	} else {
		&sendMessage(@ARGV);
		exit(0);
	}
}

# Find the script's path
use FindBin qw($Bin);
if (! -f "$Bin/legacyworlds.xml") {
	die "$0: could not find 'legacyworlds.xml' in '$Bin'\n";
}
$lwConf = "$Bin/legacyworlds.xml";

# Fork and detach
$pid = fork();
if ($pid < 0) {
	die("$0: failed to fork\n");
} elsif ($pid != 0) {
	exit(0);
}

# Detach
POSIX::setsid();
close STDIN; close STDOUT; close STDERR;
open(STDIN, "</dev/null"); open(STDOUT, ">/dev/null"); open(STDERR, ">/dev/null");

# First create the pipe if it doesn't exist
if (! -p $fifoPath) {
	if (-e $fifoPath) {
		die "$0: '$fifoPath' is not a pipe\n";
	} else {
		POSIX::mkfifo($fifoPath, 0400)
			or die "$0: unable to create '$fifoPath'\n";
	}
}
# Set the pipe's owner and group
if ($> == 0) {
	chown $fsUser, $fsGroup, $fifoPath;
} else {
	chown $>, $fsGroup, $fifoPath;
}
chmod 0620, $fifoPath;

# Create the control directory if needed
if (! -d $ctrlPath) {
	if (-e $ctrlPath) {
		die "$0: '$ctrlPath' is not a directory\n";
	} else {
		mkdir $ctrlPath, 0700
			or die "$0: unable to create '$ctrlPath'\n";
	}
}
# Set the owner and group
if ($> == 0) {
	chown $fsUser, $fsGroup, $ctrlPath;
} else {
	chown $>, $fsGroup, $ctrlPath;
}
chmod 0770, $ctrlPath;

# Define commands
%commands = (
	DIE	=> \&endController,
	MERGE	=> \&mergeConfiguration,
	TMPID	=> \&tickManagerID,
	TMINIT	=> \&tickManagerStart,
	TMSTOP	=> \&tickManagerStop,
	READY	=> \&gameReady,
	START	=> \&gameStart,
	SETEND	=> \&gameEnd,
	NOEND	=> \&gameCancelEnd,
	"END"	=> \&gameChangeEnd,
	SETDEF	=> \&setDefaultGame,
	BOTON	=> \&startBot,
	BOTOFF	=> \&killBot,
	PCPID	=> \&proxyDetectorID,
	PCON	=> \&startProxyDetector,
	PCOFF	=> \&stopProxyDetector
);

# Reader loop
while (1) {
	# Wait for someone to write to the pipe
	close(FIFO);
	open(FIFO, "< $fifoPath")
		or die "$0: unable to open '$fifoPath'\n";

	# Read it
	$command = <FIFO>;
	next unless defined $command;
	chomp($command);
	next if $command =~ /[^A-Za-z0-9\s]/;

	# Extract the actual command
	my @args = split /\s+/, $command;
	$command = shift @args;

	# Check if the command is allowed
	next unless defined $commands{$command};

	#print "Got command $command, arguments = (" . join(', ', @args) . ")\n";
	&{$commands{$command}}(@args);
}



###
## Helper functions
#

sub sendMessage {
	my @args = @_;
	die "$0: FIFO '$fifoPath' doesn't exist\n" unless -p $fifoPath;
	open(FIFO, "> $fifoPath") or die "$0: unable to open FIFO '$fifoPath'\n";
	print FIFO (join(' ', @args) . "\n");
	close(FIFO);
}


sub start {
	$pid = fork();
	if ($pid == -1) {
		die "$0: could not fork\n";
	} elsif ($pid) {
		print "LegacyWorlds - Initialising game\n";
		print " -> Controller\n";
		sleep(1);
		print " -> Proxy detector\n";
		&sendMessage("PCON");
		sleep(1);
		print " -> Ticks manager\n";
		&sendMessage("TMINIT");
		sleep(1);
		print " -> IRC bot\n";
		&sendMessage("BOTON");
		exit(0);
	}
}


sub stop {
	print "LegacyWolrds - Shutting down\n";
	print " -> IRC bot\n";
	&sendMessage("BOTOFF");
	sleep(1);
	print " -> Ticks manager\n";
	&sendMessage("TMSTOP");
	sleep(1);
	print " -> Proxy detector\n";
	&sendMessage("PCOFF");
	sleep(1);
	print " -> Controller\n";
	&sendMessage("DIE");
	exit(0);
}


###
## Command functions
#

#
# Function that terminates the controller
#
sub endController {
	exit(0);
}


#
# Function that adds a new game
#
sub mergeConfiguration {
	my $sourceFile = shift;

	return unless -f "$ctrlPath/config.$sourceFile.xml";

	# Read the new snippet
	open(NEWCONF, "< $ctrlPath/config.$sourceFile.xml");
	my @newConfiguration = <NEWCONF>;
	close(NEWCONF);

	# Read the old configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Merge it
	my @merged = ();
	foreach my $oldLine (@oldConfiguration) {
		if ($oldLine =~ /^\s+<\/Games>\s*$/) {
			@merged = (@merged, @newConfiguration, "\n");
		}
		@merged = (@merged, $oldLine);
	}

	# Write the new configuration file
	open(OLDCONF, "> $lwConf");
	print OLDCONF @merged;
	close OLDCONF;

	# Remove the source file
	unlink "$ctrlPath/config.$sourceFile.xml";
}

#
# Tick manager PID update
#
sub tickManagerID {
	my $pid = shift;
	return unless $pid;

	open(PIDFILE, "> $ctrlPath/tickManager.pid");
	print PIDFILE "$pid " . time() . "\n";
	close(PIDFILE);
}

#
# Start tick manager
#
sub tickManagerStart {
	return if &tickManagerStatus();
	return unless -f "$Bin/ticks.php";
	if ($> == 0) {
		system("su - lwticks");
	} else {
		system("cd $Bin; php ticks.php");
	}
}

#
# Stop tick manager
#
sub tickManagerStop {
	my $pid;
	return unless ($pid = &tickManagerStatus());
	kill 15, $pid;
	unlink("$ctrlPath/tickManager.pid");
}

#
# Start IRC bot
#
sub startBot {
	&killBot();
	if ($> == 0) {
		system("su - lwbot");
	} else {
		system("cd $Bin/../ircbot; (php bot.php &) </dev/null >/dev/null 2>&1");
	}
}

#
# Stop IRC bot
#
sub killBot {
	return unless -f "$ctrlPath/ircbot.pid";
	my $pid = `cat $ctrlPath/ircbot.pid`;
	kill 15, $pid;
	unlink("$ctrlPath/ircbot.pid");
}

#
# Check tick manager status
#
sub tickManagerStatus {
	return 0 unless -f "$ctrlPath/tickManager.pid";

	open(PIDFILE, "< $ctrlPath/tickManager.pid");
	my $data = <PIDFILE>;
	close(PIDFILE);

	chomp($data);
	my ($pid, $time) = split / /, $data;
	return (time() - $time < 22 ? $pid : 0);
}

#
# Make a game ready
#
sub gameReady {
	my $gName = shift;

	# Read the current configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Generate new configuration
	my @newConf = ();
	foreach my $line (@oldConfiguration) {
		if ($line =~ /<Game\s+id="$gName".*\s+public="0"\s+canjoin="0"/) {
			$line =~ s/\spublic="0"\s+canjoin="0"/ public="1" canjoin="1"/;
		}
		push @newConf, $line;
	}

	# Write configuration file
	open(NEWCONF, "> $lwConf");
	print NEWCONF @newConf;
	close(NEWCONF);
}

#
# Start the game earlier or later
#
sub gameStart {
	my $gName = shift;
	my $when = shift;

	return if ($when ne "EARLY" && $when ne "LATE");
	$when = ($when eq 'EARLY') ? -1 : 1;
	$when *= 24 * 60 * 60;

	# Read the current configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Generate new configuration
	my @newConf = ();
	my $state = 0;
	foreach my $line (@oldConfiguration) {
		if ($state == 0 && $line =~ /<Game\s+id="$gName".*\s+public="1"\s+canjoin="1"/) {
			$state = 1;
		} elsif ($state == 1) {
			if ($line =~ /<\/Game>/) {
				$state = 0;
			} elsif ($line =~ /<Tick\s+script="[a-z]+"\s+first="([0-9]+)"\s+interval="[0-9]+"\s*\/>/) {
				my $fTick = $1;
				$fTick += $when;
				$line =~ s/\sfirst="[0-9]+"/ first="$fTick"/;
			}
		}
		push @newConf, $line;
	}

	# Write configuration file
	open(NEWCONF, "> $lwConf");
	print NEWCONF @newConf;
	close(NEWCONF);
}

#
# Set a running game to end
#
sub gameEnd {
	my $gName = shift;
	my $when = shift;

	$when = $when * 60 * 60 + time();

	# Read the current configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Generate new configuration
	my @newConf = ();
	my $state = 0;
	foreach my $line (@oldConfiguration) {
		if ($state == 0 && $line =~ /<Game\s+id="$gName".*\s+public="1"\s+canjoin="1"/) {
			$state = 1;
		} elsif ($state == 1) {
			if ($line =~ /<\/Game>/) {
				$state = 0;
			} elsif ($line =~ /<Tick\s+script="[a-z]+"\s+first="([0-9]+)"\s+interval="[0-9]+"\s*\/>/) {
				my $fTick = $1;
				$line =~ s/\sfirst="[0-9]+"/ first="$fTick" last="$when"/;
			}
		}
		push @newConf, $line;
	}

	# Write configuration file
	open(NEWCONF, "> $lwConf");
	print NEWCONF @newConf;
	close(NEWCONF);
}

#
# Set an ending game's status back to running/victory
#
sub gameCancelEnd {
	my $gName = shift;

	# Read the current configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Generate new configuration
	my @newConf = ();
	my $state = 0;
	foreach my $line (@oldConfiguration) {
		if ($state == 0 && $line =~ /<Game\s+id="$gName".*\s+public="1"\s+canjoin="1"/) {
			$state = 1;
		} elsif ($state == 1) {
			if ($line =~ /<\/Game>/) {
				$state = 0;
			} elsif ($line =~ /<Tick\s+script="[a-z]+"\s+/) {
				$line =~ s/\slast="[0-9]+"//;
			}
		}
		push @newConf, $line;
	}

	# Write configuration file
	open(NEWCONF, "> $lwConf");
	print NEWCONF @newConf;
	close(NEWCONF);
}

#
# End the game earlier or later
#
sub gameChangeEnd {
	my $gName = shift;
	my $when = shift;

	return if ($when ne "EARLY" && $when ne "LATE" && $when ne "NOW");
	if ($when ne 'NOW') {
		$when = ($when eq 'EARLY') ? -1 : 1;
		$when *= 24 * 60 * 60;
	}

	# Read the current configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Generate new configuration
	my @newConf = ();
	my $state = 0;
	foreach my $line (@oldConfiguration) {
		if ($state == 0 && $line =~ /<Game\s+id="$gName".*\s+public="1"\s+canjoin="1"/) {
			$state = 1;
		} elsif ($state == 1) {
			if ($line =~ /<\/Game>/) {
				$state = 0;
			} elsif ($line =~ /<Tick\s+script="[a-z]+".*\slast="([0-9]+)"/) {
				my $lTick = $1;
				if ($when eq 'NOW') {
					$lTick = time();
				} else {
					$lTick += $when;
				}
				$line =~ s/\slast="[0-9]+"/ last="$lTick"/;
			}
		}
		push @newConf, $line;
	}

	# Write configuration file
	open(NEWCONF, "> $lwConf");
	print NEWCONF @newConf;
	close(NEWCONF);
}

#
# Changes the default game
#
sub setDefaultGame {
	my $gName = shift;
	return if ($gName eq "");

	# Read the current configuration
	open(OLDCONF, "< $lwConf");
	my @oldConfiguration = <OLDCONF>;
	close(OLDCONF);

	# Generate new configuration
	my @newConf = ();
	foreach my $line (@oldConfiguration) {
		if ($line =~ /<Games\s+default="[a-z0-9]+"/) {
			$line =~ s/<Games\s+default="[0-9a-z]+"/<Games default="$gName"/;
		}
		push @newConf, $line;
	}

	# Write configuration file
	open(NEWCONF, "> $lwConf");
	print NEWCONF @newConf;
	close(NEWCONF);
}

#
# Proxy detector PID update
#
sub proxyDetectorID {
	my $pid = shift;
	return unless $pid;

	open(PIDFILE, "> $ctrlPath/proxyDetector.pid");
	print PIDFILE "$pid " . time() . "\n";
	close(PIDFILE);
}

#
# Start proxy detector
#
sub startProxyDetector {
	return if &proxyDetectorStatus();
	return unless -f "$Bin/proxycheck.php";
	if ($> == 0) {
		system("su - lwproxy");
	} else {
		system("cd $Bin; php proxycheck.php");
	}
}

#
# Stop proxy detector
#
sub stopProxyDetector {
	my $pid;
	return unless ($pid = &proxyDetectorStatus());
	kill 15, $pid;
	unlink("$ctrlPath/proxyDetector.pid");
}

#
# Check proxy detector status
#
sub proxyDetectorStatus {
	return 0 unless -f "$ctrlPath/proxyDetector.pid";

	open(PIDFILE, "< $ctrlPath/proxyDetector.pid");
	my $data = <PIDFILE>;
	close(PIDFILE);

	chomp($data);
	my ($pid, $time) = split / /, $data;
	return (time() - $time < 22 ? $pid : 0);
}
