cd ~/sphenix/single/truthconversion/build
make install
if [ $? -eq 0 ]; then
  cd ../../condor
  if [ "$1" = "submit_Cluster" ]; then
    condor_submit submit_Cluster
  elif [ "$1" = "CondorClusterLocal.csh" ]; then
    CondorClusterLocal.csh
  fi
fi
