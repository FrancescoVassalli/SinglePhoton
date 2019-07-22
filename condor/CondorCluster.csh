#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set OUT_FILE=conversiononlineanalysis${p}.root
set IN_FILE=conversionout${p}.root
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_photons${p}"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="condor/cluster_burner.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
#root -b -q Fun4All_G4_sPHENIX.C\(1000,\"$IN_FILE\"\) 
#cp -f $IN_FILE $OUT_LOCATION$IN_FILE
cp $OUT_LOCATION$IN_FILE .
root -b -q cluster_burner.C\(\"$IN_FILE\",\"$OUT_FILE\",$p\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
