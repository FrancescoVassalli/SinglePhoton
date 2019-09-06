#!/bin/csh 
@ p =  ${1} + 500
#
set OUT_LOCATION="/sphenix/user/vassalli/minBiasPythia/"
set PYTHIA_FILE=${OUT_LOCATION}pythia_hard4_${p}
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/pythia/*"
set GENNAME="hardGenerator"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA/fran_single_photons
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/fran_single_photons/
#
cd $SCRATCH_AREA/fran_single_photons
./$GENNAME $PYTHIA_FILE 100000
#
rm -r $SCRATCH_AREA/fran_single_photons
#
exit 0
