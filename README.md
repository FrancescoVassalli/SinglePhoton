#SinglePhoton
## This repository is used to generate and analyze photon conversion in G4 sPHENIX simulations.
**generation**: folders used to generate the events look at the readme's in the subfolder for better understanding 
- gen: the Fun4All control files used to generate events updated to the most recent coresoftware release that I have run on 
- condor: csh and submit scripts for condor to call the Fun4All modules to create events and afterburners to process them
**process**: the after burners to process the events from DSTs to useful roots 
- SinglePhotonAfter: records basic truth information in a TTree and records convesion assosiated clusters
- PhotonConversion: recover photons and record them in a vector and TTree
- ClusterShape: processes the data in the simulation to an easy TTree of cluster information which is formated by @ChaseSmith
**analysis**: used to make and plot plots about the coversions 
- analysis: contains the files used for extracting data from the root trees and plotting them

**Background on Photon Conversion**
- I have been using ![this ATLAS paper](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf) as a reference. The relevent information starts on page 136 of the pdf

**Conversion Efficiency** 
- At begin I studied the exiting efficiency of the sPHENIX simulation to recover converted photons. These two plots were made on 12/3/18 and describe the effiency relative to radius and pT for single photon events using SvtxTrack reconstruction. This is also known as the inside out method. 
![pT](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf)
![radius](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecoR.pdf)

**Truth Conversion**
- My work on using truth information to match conversions to EM clusters as been collected into a single package which is the truthconversion folder in this repository. I believe these clusters will be useful in determining how to implement the outside-in method in SPHENIX. 
