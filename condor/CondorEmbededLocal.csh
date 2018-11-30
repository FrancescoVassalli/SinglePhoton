#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set OUT_FILE=${OUT_LOCATION}fourembededonlineanalysis${p}.root
set IN_FILE="/sphenix/user/vassalli/gammasample/fourembededout${p}.root"
set PYTHIA_FILE="/sphenix/user/vassalli/gammasample/pythiahep.dat"
#
set SCRATCH_AREA="/sphenix/user/vassalli/scratch"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="./after_embeded.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
#
cd $SCRATCH_AREA
#root -b -q Fun4All_G4_sPHENIX.C\(10,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
root -b -q after_embeded.C\(\"$IN_FILE\",\"$OUT_FILE\"\)
#
rm -rf $SCRATCH_AREA
#
exit 0
