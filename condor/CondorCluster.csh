#!/bin/csh 
@ p = ( ${1} )
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/"
set OUT_FILE=conversiononlineanalysis${p}.root
set IN_FILE=conversionout${p}.root
set PYTHIA_FILE="/sphenix/user/vassalli/gammasample/pythiahep.dat"
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_photons"                                                                                                              
#
set SOURCE_PHOTONMAKER="/direct/phenix+u/vassalli/sphenix/single/gen/*"
set BURNER="condor/cluster_burner.C"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp  $SOURCE_PHOTONMAKER $SCRATCH_AREA
cp $BURNER $SCRATCH_AREA
#
cd $SCRATCH_AREA
root -b -q Fun4All_G4_sPHENIX.C\(10,\"$IN_FILE\"\) 
root -b -q cluster_burner.C\(\"$IN_FILE\",\"$OUT_FILE\",$p\)
cp $OUTFILE $OUTLOCATION$OUTFILE
cp $INFILE $OUTLOCATION$INFILE
#
rm -rf $SCRATCH_AREA
#
exit 0
