#!/bin/csh 
@ p = $1
#
set OUTLOCATION="/sphenix/user/vassalli/G4SinglePhoton/"
set OUTNAME="out${1}.root"
set INLOCATION="/sphenix/user/chase/clusterProbTest"
set INNAME="photon_DST_${1}.root"
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"
set OUT_FILE="$OUTLOCATION/$OUTNAME"
set IN_FILE="$INLOCATION/$INNAME"
#
set BURNER="condor/after_macro.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA/fran_single_photons
cp $BURNER $SCRATCH_AREA/fran_single_photons/
#
cd $SCRATCH_AREA/fran_single_photons
root -b -q after_macro.C\(\"$IN_FILE\",\"$OUT_FILE\"\)
#
rm -r $SCRATCH_AREA/fran_single_photons
#
exit 0
