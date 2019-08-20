#!/usr/bin/perl -w
#inspired by @sh-lim
use Cwd;
use File::Copy qw(copy);
#
#$package = "pp200GeV_inelastic";
#$package = "pp200GeV_hardqcd";
#$package = "pp200GeV_softqcd";
#$package = "pp7000GeV_hardqcd";
#$package = "pp7000GeV_softqcd";
#$package = "pp13TeV_softqcd";
$package = "minBiasConversion";
$maindir = "/sphenix/user/vassalli";
$logpath = "/sphenix/user/vassalli/condor_logs/";
$driver = "CondorAfterMinBiasPythia.csh";
#
$groupnum = 2;
#
$rundir = "${maindir}/${package}/grp${groupnum}";
mkdir $rundir;
#
for ($irun=0; $irun<30; $irun++){
  $wait =int(rand(2)); 
  print "Wait=${wait}\n";
  sleep $wait;
  #
  $wrkdir = "${rundir}_${irun}";
  mkdir $wrkdir;
  copy $driver, $wrkdir;
  chdir $wrkdir;
  chmod 0777, $driver or die;
  open(FILE, ">condor");
  print FILE "Universe = vanilla\n";
  print FILE "Notification = Never\n";
  print FILE "Arguments = \$(Process) ${wrkdir}/\n";
  print FILE "Requirements = CPU_Speed>=1\n";
  print FILE "Rank = CPU_Speed\n";
  print FILE "Priority = +18\n";
  print FILE "run_as_owner = True\n";
  print FILE "Executable = $driver\n";
  print FILE "Log = ${logpath}log.auto${groupnum}.\$(Process)\n";
  print FILE "Output = ${logpath}out.auto${groupnum}.\$(Process)\n";
  print FILE "Error = ${logpath}err.auto${groupnum}.\$(Process)\n";
  print FILE "Notify_user = frva5829\@colorado.edu\n";
  print FILE "request_memory = 3000M\n";
  # print FILE "+Experiment = \"phenix\"\n";
  # print FILE "+Job_Type = \"cas\"\n";
  print FILE "Queue 100\n";
  close(FILE);
  system "condor_submit condor";
}
chdir "${maindir}/${package}";
#AnaAdder.bash;
