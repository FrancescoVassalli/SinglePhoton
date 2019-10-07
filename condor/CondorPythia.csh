#!/bin/csh 
@ p =  ${1}
#
set OUT_LOCATION="/sphenix/user/vassalli/minBiasPythia/"
set PYTHIA_FILE=${OUT_LOCATION}pythia_hard4_${p}
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_photons${p}"
#
set GENNAME="hardGenerator"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp ./pythia/$GENNAME $SCRATCH_AREA
#
cd $SCRATCH_AREA
./$GENNAME $PYTHIA_FILE 2000000
#
rm -r $SCRATCH_AREA
#
exit 0
