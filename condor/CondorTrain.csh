#!/bin/csh 
source /phenix/u/vassalli/.cshrc
set TRAINER = "vtxRS.C"
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/training"
mkdir $SCRATCH_AREA
#
cp $TRAINER $SCRATCH_AREA
cd $SCRATCH_AREA
/cvmfs/sphenix.sdcc.bnl.gov/x8664_sl7/opt/sphenix/core/root-5.34.38/bin/root -b -q $TRAINER\(\)
cp -rf $SCRATCH_AREA $HOME/sphenix/single/Training/condorout
rm -rf $SCRATCH_AREA
#
exit 0
