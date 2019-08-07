#!/bin/csh 
@ p =  ${1}
#
set OUT_LOCATION="/sphenix/user/vassalli/minBiasPythia/"
set OUT_FILE=conversionembededminBiasanalysis${p}.root
set IN_FILE=minBiasembededout${p}.root
set PYTHIA_FILE=tempPythiaMinBiasHep${p}.dat
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_minBias${p}"                                             
#
set SOURCE_FUN4ALL="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set SOURCE_PYTHIA="/direct/phenix+u/vassalli/sphenix/single/pythia/generator"
set BURNER="condor/after_Reco.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_FUN4ALL $SCRATCH_AREA
cp  $SOURCE_PYTHIA $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
@ NEVENTS = 200
./generator $PYTHIA_FILE $NEVENTS
root -b -q Fun4All_G4_MinBias.C\($NEVENTS,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
cp -f $IN_FILE $OUT_LOCATION$IN_FILE
root -l -b -q after_Reco.C\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\"\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
