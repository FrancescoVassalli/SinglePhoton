#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set OUT_FILE=Konlineanalysis${p}.root
set IN_FILE=Kout${p}.root
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_K${p}"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="condor/after_embeded.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
root -b -q Fun4All_G4_sPHENIX.C\(100,\"$IN_FILE\"\) 
cp -f $IN_FILE $OUT_LOCATION$IN_FILE
root -b -q after_embeded.C\(\"$OUT_LOCATION$IN_FILE\",\"$OUT_FILE\",$p\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
rm -rf $SCRATCH_AREA
#
exit 0
