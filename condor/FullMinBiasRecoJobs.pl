#!/usr/bin/perl -w
#inspired by @sh-lim
use Cwd;

#$package = "pp200GeV_inelastic";
#$package = "pp200GeV_hardqcd";
#$package = "pp200GeV_softqcd";
#$package = "pp7000GeV_hardqcd";
#$package = "pp7000GeV_softqcd";
#$package = "pp13TeV_softqcd";
$package = "minBiasConversion";
$maindir = "/sphenix/user/vassalli";
$logpath = "/direct/phenix+u/vassalli/sphenix/single/condor_logs/";

$groupnum = 0;

$rundir = "${maindir}/${package}/grp${groupnum}";
mkdir $rundir;

for ($irun=0; $irun<100; $irun++){

	sleep 5+int(rand(60));

	$wrkdir = "${rundir}/${irun}";
	mkdir $wrkdir;

	chdir $wrkdir;
	open(FILE, ">condor");
	print FILE "Universe = vanilla\n";
	print FILE "Notification = Never\n";
	print FILE "Arguments = \$(Process) ${wrkdir}\n";
	print FILE "Requirements = CPU_Speed>=1\n";
	print FILE "Rank = CPU_Speed\n";
	print FILE "Priority = +1\n";
	print FILE "Executable = CondorMinBiasPythia\n";
	print FILE "Log = ${logpath}log.auto.\$(Process)\n";
	print FILE "Output = ${logpath}out.auto.\$(Process)\n";
	print FILE "Error = ${logpath}err.auto.\$(Process)\n";
	print FILE "Notify_user = frva5829\@colorado.edu\n";
#	print FILE "+Experiment = \"phenix\"\n";
#	print FILE "+Job_Type = \"cas\"\n";
	print FILE "Queue 400\n";
	close(FILE);

	system "condor_submit condor";
}

