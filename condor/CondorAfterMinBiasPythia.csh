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
set BURNERNAME="after_embeded.C"
set BURNER="/direct/phenix+u/vassalli/sphenix/single/condor/${BURNERNAME}"
set SOURCE_BURNER="/direct/phenix+u/vassalli/sphenix/single/truthconversion/*"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_FUN4ALL $SCRATCH_AREA
cp  $SOURCE_PYTHIA $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp $SOURCE_BURNER $SCRATCH_AREA
#
cd $SCRATCH_AREA
@ NEVENTS = 200
./generator $PYTHIA_FILE $NEVENTS
set PYTHIA_FILE = $PYTHIA_FILE".dat"
#cp -f $PYTHIA_FILE $OUT_LOCATION$PYTHIA_FILE
#root -b -q Fun4All_G4_MinBias.C\($NEVENTS,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
#cp -f $IN_FILE $OUT_LOCATION$IN_FILE
root -l -b -q $BURNERNAME\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\"\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
