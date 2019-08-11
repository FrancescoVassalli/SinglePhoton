#!/usr/bin/perl -w
#inspired by @sh-lim
use Cwd;
use File::Copy qw(copy);

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
  $wait =5+int(rand(60)); 
  print "Wait=${wait}\n";
  sleep $wait;

	$wrkdir = "${rundir}_${irun}";
	mkdir $wrkdir;
  copy "CondorMinBiasPythia.csh", $wrkdir;
  chdir $wrkdir;
  chmod "777 CondorMinBiasPythia,csh";
	open(FILE, ">condor");
	print FILE "Universe = vanilla\n";
	print FILE "Notification = Never\n";
	print FILE "Arguments = \$(Process) ${wrkdir}/\n";
	print FILE "Requirements = CPU_Speed>=1\n";
	print FILE "Rank = CPU_Speed\n";
	print FILE "Priority = +20\n";
	print FILE "Executable = CondorMinBiasPythia.csh\n";
	print FILE "Log = ${logpath}log.auto${irun}.\$(Process)\n";
	print FILE "Output = ${logpath}out.auto${irun}.\$(Process)\n";
	print FILE "Error = ${logpath}err.auto${irun}.\$(Process)\n";
  print FILE "Notify_user = frva5829\@colorado.edu\n";
	print FILE "request_memory = 3500M\n";
#	print FILE "+Experiment = \"phenix\"\n";
#	print FILE "+Job_Type = \"cas\"\n";
	print FILE "Queue 100\n";
	close(FILE);
	system "condor_submit condor";
}

