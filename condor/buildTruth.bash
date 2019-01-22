cd ~/sphenix/single/truthconversion/build
make install
if [ $? -eq 0]; then
  cd ../../condor
  if [ $1 -eq "submit_cluster" ]; then
    condor_submit submit_cluster
  elif [ $1 -eq "CondorClusterLocal.csh" ]; then
    CondorClusterLocal.csh
  fi
fi
