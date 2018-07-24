#!/bin/csh 
@ p = $1
#
set OUT_FILE="/sphenix/user/vassalli/G4SinglePhoton/out${1}.root"
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA/fran_single_photons
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/fran_single_photons/
#
cd $SCRATCH_AREA/fran_single_photons
root -b -q Fun4All_G4_sPHENIX.C\(\"$OUT_FILE\"\) 
root -b -q after_macro.C\(\"$OUT_FILE\"\)
#
rm -r $SCRATCH_AREA/fran_single_photons
#
exit 0
