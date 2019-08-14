#!/bin/csh 
@ p =  ${1}
#
set OUT_LOCATION="/sphenix/user/vassalli/minBiasPythia/"
set OUT_FILE=conversionembededminBiasanalysis${p}.root
set IN_FILE=minBiasembededout${p}.root
set PYTHIA_FILE="/sphenix/user/vassalli/minBiasPythia/pythiaMinBiasHep${p}.dat.dat"
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_minBias${p}"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="condor/after_Reco.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
#root -b -q Fun4All_G4_MinBias.C\(100,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
#cp -f $IN_FILE $OUT_LOCATION$IN_FILE
root -l -b -q after_Reco.C\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\"\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
