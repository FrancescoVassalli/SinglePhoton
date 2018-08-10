#!/bin/csh 
#
set OUT_FILE="/sphenix/user/vassalli/singlesamples/Photon5/onlineanalysis${1}.root"
set IN_FILE="/sphenix/user/vassalli/singlesamples/Photon5/out${1}.root"
#
#
set BURNER="./after_DST.C"
#
source /phenix/u/vassalli/.cshrc
#
root -b -q after_DST.C\(\"$IN_FILE\",\"$OUT_FILE\"\)
#
exit 0
