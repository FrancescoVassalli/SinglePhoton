#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set OUT_FILE=conversionHIonlineanalysis${p}.root
set IN_FILE=conversionHIout${p}.root
#set HI_FILE="/sphenix/user/vassalli/gammasample/pythiahep.dat"
set SCRATCH_AREA="srtch"                                                               
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="after_embed.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
cp ../truthconversion/* $SCRATCH_AREA
#
cd $SCRATCH_AREA
root -b -q Fun4All_G4_sPHENIX.C\(5,\"$IN_FILE\",\"$HI_FILE\"\) 
cp -f $IN_FILE $OUT_LOCATION$IN_FILE
#cp $OUT_LOCATION$IN_FILE .
root -b -q after_embeded.C\(\"$IN_FILE\",\"$OUT_FILE\"\)
cp $OUT_FILE $OUT_LOCATION$OUT_FILE
#
exit 0
