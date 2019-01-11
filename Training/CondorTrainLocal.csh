#!/bin/csh 
@ p = ( ${1} )
#
#/sphenix/user/vassalli/gammasample/test/fourembededout0.root
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set IN_FILE=${OUT_LOCATION}fourembededonlineanalysis${p}.root
set OUT_FILE=${OUT_LOCATION}cutTraining${p}.root
#
set SCRATCH_AREA="/sphenix/user/vassalli/scratch"                                                                                                              
#
set BURNER="train.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
#
cd $SCRATCH_AREA
#
root -b -q $BURNER\(\"$IN_FILE\",\"$OUT_FILE\",$p\)
#
rm -rf $SCRATCH_AREA
#
exit 0
