#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set OUT_FILE=${OUT_LOCATION}/test/fourembededonlineanalysis${p}.root
set IN_FILE=${OUT_LOCATION}fourembededout${p}.root
set PYTHIA_FILE="/sphenix/user/vassalli/gammasample/pythiahep.dat"
#
set SCRATCH_AREA="/sphenix/user/vassalli/scratch"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNERPATH="../SVReco/"
set BURNERNAME="Run_SVReco.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNERPATH/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
#root -b -q Fun4All_G4_sPHENIX.C\(5,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
root -b -q $BURNERNAME\(5,\"$IN_FILE\"\)
#
rm -rf $SCRATCH_AREA
#
exit 0
