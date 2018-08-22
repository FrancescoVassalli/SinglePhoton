#!/bin/csh 
@ p = ( ${1} )
#
set LD_LIBRARY_PATH=$LD_LIB_NOLOCAL
set OUT_LOCATION="/sphenix/user/vassalli/singlesamples/Photon5/test/"
set OUT_FILE=${OUT_LOCATION}onlineanalysis${p}.root
set IN_FILE="/sphenix/user/vassalli/singlesamples/Photon5/test/out${p}.root"
#
set SCRATCH_AREA="/sphenix/user/vassalli/scratch"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/FunFriends/*"
set BURNER="./after_DST.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA/
cp $BURNER $SCRATCH_AREA/
#
rm $OUTFILE
cd $SCRATCH_AREA
root -b Fun4All_G4_sPHENIX.C\(5,\"$IN_FILE\"\) 
echo $LD_LIBRARY_PATH
root -b -q after_DST.C\(\"$IN_FILE\",\"$OUT_FILE\"\)
#
rm -rf $SCRATCH_AREA
#
exit 0
