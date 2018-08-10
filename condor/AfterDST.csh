#!/bin/csh 
@ p = $1
#
set OUT_FILE="/sphenix/user/vassalli/singlesamples/Photon5/onlineanalysis${1}.root"
set IN_FILE="/sphenix/user/vassalli/singlesamples/Photon5/out${1}.root"
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR"                                                                                                              
#
set BURNER="condor/after_DST.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA/fran_single_photons
cp $BURNER $SCRATCH_AREA/fran_single_photons/
#
cd $SCRATCH_AREA/fran_single_photons
echo run $1
if($1 > 100) then
  root -b -q after_DST.C\(\"$IN_FILE\",\"$OUT_FILE\"\)
endif
#
rm -r $SCRATCH_AREA/fran_single_photons
#
exit 0
