#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/embeded/"
set ANA_LOCATION="/sphenix/user/vassalli/gammasample/embeded/"
set OUT_FILE=conversionembededonlineanalysis${p}.root
set IN_FILE=conversionembededout${p}.root
set PYTHIA_FILE=${OUT_LOCATION}/pythiahep/pythia_photonJet_${p}.dat
#
set SCRATCH_AREA="/sphenix/user/vassalli/srtch"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="./after_*.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp ../truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
#root -b -q Fun4All_G4_sPHENIX.C\(100,\"$IN_FILE\",\"$PYTHIA_FILE\"\) 
#cp -f $IN_FILE $OUT_LOCATION$IN_FILE
#root -l -b -q after_Reco.C\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\"\)
#cp -f $OUT_FILE $ANA_LOCATION$OUT_FILE
root -l -b -q after_embeded.C\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\"\)
cp -f $OUT_FILE $ANA_LOCATION"truth"$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
