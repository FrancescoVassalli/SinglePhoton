#!/bin/csh 
source /phenix/u/vassalli/.cshrc
set SCRATCH_AREA="$_CONDOR_SCRATCH_DIR/training"
mkdir $SCRATCH_AREA
#
cp -r train.C $SCRATCH_AREA
cd $SCRATCH_AREA
root -b -q train.C\(\)
cd -
cp -rf $SCRATCH_AREA .
rm -rf $SCRATCH_AREA
#
exit 0
