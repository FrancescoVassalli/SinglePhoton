#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION= ( ${2} )
echo $OUT_LOCATION
set OUT_FILE=conversionembededminBiasanalysis${p}.root
set IN_FILE=minBiasembededout${p}.root
set PYTHIA_FILE=tempPythiaMinBiasHep${p}
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/srcth${p}"                                             
#
set SOURCE_FUN4ALL="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set SOURCE_PYTHIA="/direct/phenix+u/vassalli/sphenix/single/pythia/generator"
set BURNER="$HOME/sphenix/single/condor/after_Reco.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_FUN4ALL $SCRATCH_AREA
cp  $SOURCE_PYTHIA $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp ../truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
@ NEVENTS = 200
./generator $PYTHIA_FILE $NEVENTS
set PYTHIA_FILE = $PYTHIA_FILE".dat"
#cp -f $PYTHIA_FILE $OUT_LOCATION$PYTHIA_FILE
root -b -q Fun4All_G4_MinBias.C\($NEVENTS,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
cp -f $IN_FILE $OUT_LOCATION$IN_FILE
root -l -b -q after_Reco.C\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\"\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
