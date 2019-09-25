#!/bin/csh 
@ p =  ${1}
#
set OUT_LOCATION="/sphenix/user/vassalli/gammasample/pythiahep/"
set PYTHIA_FILE=${OUT_LOCATION}pythia_photonJet_${p}
#
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/fran_photons${p}"
#
set GENNAME="photonJetGenerator"
#
source /phenix/u/vassalli/.cshrc
mkdir $SCRATCH_AREA
cp ./pythia/$GENNAME $SCRATCH_AREA
#
cd $SCRATCH_AREA
./$GENNAME $PYTHIA_FILE 100
#
rm -r $SCRATCH_AREA
#
exit 0
